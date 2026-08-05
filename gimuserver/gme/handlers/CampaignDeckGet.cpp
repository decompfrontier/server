#include "App.hpp"
#include "Handlers.hpp"

#include <gimuserver/gme/common/Common.hpp>

// CampaignDeckGet (C3a0VnQK) — returns the campaign-specific party deck
// composition for the deck-select/edit screen.
//
// Response key "w5vHLT0q" — CampaignMissionPartyDeckInfoResponse:
//   [{ "zsiAn9P1": "<DeckNum>",  "edy7fq3L": "<UserUnitId>",
//      "gr48vsdJ": "<MemberType>", "XuJL4pc5": "<Disporder>",
//      "a3qJ6QhX": "<NowHp>",    "3WMz78t6":  "<MaxHp>",
//      "gU2xtQ0V": "<BbGauge>",  "nIGZ1X9C":  "<SbGauge>" }, … ]

// CampaignDeckMember + CampaignDeckGetResp are generated from the KDL
// (packet-generator/assets/net/handlers.kdl).

HANDLEF(CampaignDeckGet)
{
    LOG_INFO << "CampaignDeckGet: " << json;

    CampaignDeckGetReq req{};
    {
        glz::context ctx{};
        if (const auto ec = glz::read<glz::opts{.error_on_unknown_keys = false}>(req, json, ctx); ec)
            LOG_WARN << "CampaignDeckGet: parse error: " << glz::format_error(ec, json);
    }

    const auto identity = (co_await gme::getUserIdentity(theDb(), req.login_info)).nonEmpty();
    const std::string kUserId = identity.userId;

    CampaignDeckGetResp resp{};

    try
    {
        auto rows = co_await theDb()->execSqlCoro(
            "SELECT deck_num, user_unit_id, member_type, disporder"
            " FROM user_campaign_decks WHERE user_id=$1 ORDER BY deck_num, disporder;",
            std::string(kUserId));

        // Fall back to regular party deck when campaign deck is not yet configured.
        if (rows.empty())
        {
            rows = co_await theDb()->execSqlCoro(
                "SELECT deck_num, user_unit_id, member_type, disp_order AS disporder"
                " FROM user_decks WHERE user_id=$1 ORDER BY deck_num, disp_order;",
                std::string(kUserId));
        }

        resp.members.reserve(rows.size());
        for (const auto& r : rows)
        {
            CampaignDeckMember m{};
            m.deck_num     = r["deck_num"].as<int32_t>();
            m.user_unit_id = r["user_unit_id"].as<int32_t>();
            m.member_type  = r["member_type"].as<int32_t>();
            m.disporder    = r["disporder"].as<int32_t>();
            // HP/gauge left at 0 — client fills from its unit cache on entry.
            resp.members.emplace_back(std::move(m));
        }
    }
    catch (const drogon::orm::DrogonDbException& ex)
    {
        LOG_WARN << "CampaignDeckGet: SELECT failed: " << ex.base().what();
        co_return HandleResult::success("{}");
    }

    co_return HandleResult::success(glz::write_json(resp).value_or("{}"));
}
