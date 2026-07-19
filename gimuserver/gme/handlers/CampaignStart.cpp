#include "App.hpp"
#include "Handlers.hpp"

#include <gimuserver/gme/common/Common.hpp>

// CampaignStart (6Y0gaPQN) — fired when the player opens the Campaign menu.
// Returns the mission catalog (state, progress), party deck list, item slots,
// and misc event flags so the UI can render the mission-select screen.
//
// Response keys (from typed_responses/*.hpp):
//   "2I9V0o6J" — CampaignMissionInfoResponse  (mission state list)
//   "hT95cq8K" — CampaignPartyDeckListResponse (party decks for select screen)
//   "NjZ6ds1S" — CampaignEqpItemInfoResponse   (equipped items — empty OK)
//   "5EByfWJ4" — CampaignRsvItemInfoResponse    (reserve items — empty OK)
//   "Yusr3Zg5" — CampaignMissionDeckInfoResponse
//   "RseDpY04" — CampaignMissionEventInfoResponse (single-object flags)
//   "p04iC2wr" — CampaignRewardBonusInfoResponse (empty OK)

// The response structs are generated from the KDL
// (packet-generator/assets/net/handlers.kdl): CampaignStartResp and its
// entries CampaignMissionEntry (2I9V0o6J, shared with CampaignMissionGet),
// CampaignStartDeckEntry (hT95cq8K), CampaignStartMissionDeckEntry (Yusr3Zg5),
// CampaignStartEventInfo (RseDpY04), and CampaignEmptyEntry (the always-empty
// NjZ6ds1S / 5EByfWJ4 / p04iC2wr slots).

// ---------------------------------------------------------------------------
HANDLEF(CampaignStart)
{
    LOG_INFO << "CampaignStart: " << json;

    CampaignStartReq req{};
    {
        glz::context ctx{};
        if (const auto ec = glz::read<glz::opts{.error_on_unknown_keys = false}>(req, json, ctx); ec)
            LOG_WARN << "CampaignStart: parse error: " << glz::format_error(ec, json);
    }

    const auto identity = (co_await gme::getUserIdentity(theDb(), req.login_info)).nonEmpty();
    const std::string kUserId = identity.userId;

    CampaignStartResp resp{};

    // Load mission progress.
    try
    {
        const auto rows = co_await theDb()->execSqlCoro(
            "SELECT mission_id, attain_percent, state"
            " FROM user_campaign_missions WHERE user_id=$1;",
            std::string(kUserId));

        resp.missions.reserve(rows.size());
        for (const auto& r : rows)
        {
            CampaignMissionEntry e{};
            e.mission_id     = r["mission_id"].as<std::string>();
            e.attain_percent = r["attain_percent"].as<int32_t>();
            e.state          = r["state"].as<int32_t>();
            e.mission_on_flg = (e.state >= 1) ? "1" : "0";
            resp.missions.emplace_back(std::move(e));
        }
    }
    catch (const drogon::orm::DrogonDbException& ex)
    {
        LOG_WARN << "CampaignStart: mission SELECT failed: " << ex.base().what();
    }

    // Load campaign party decks.  If user_campaign_decks is empty (not yet
    // edited), fall back to the regular user_decks so the deck panel
    // isn't blank on first launch.
    try
    {
        auto rows = co_await theDb()->execSqlCoro(
            "SELECT deck_num, member_type, user_unit_id, disporder"
            " FROM user_campaign_decks WHERE user_id=$1 ORDER BY deck_num, disporder;",
            std::string(kUserId));

        if (rows.empty())
        {
            rows = co_await theDb()->execSqlCoro(
                "SELECT deck_num, member_type, user_unit_id, disp_order AS disporder"
                " FROM user_decks WHERE user_id=$1 ORDER BY deck_num, disp_order;",
                std::string(kUserId));
        }

        for (const auto& r : rows)
        {
            CampaignStartDeckEntry e{};
            e.deck_num    = r["deck_num"].as<int32_t>();
            e.member_type = r["member_type"].as<int32_t>();
            resp.party_decks.emplace_back(std::move(e));
        }
    }
    catch (const drogon::orm::DrogonDbException& ex)
    {
        LOG_WARN << "CampaignStart: deck SELECT failed: " << ex.base().what();
    }

    // Populate mission_decks (Yusr3Zg5) — one entry per deck slot (0-9) so
    // the campaign deck-selector panel has something to render.
    for (int i = 0; i < 10; ++i)
    {
        CampaignStartMissionDeckEntry md{};
        md.deck_num      = i;
        md.now_point_num = 0;
        resp.mission_decks.emplace_back(md);
    }

    co_return HandleResult::success(glz::write_json(resp).value_or("{}"));
}
