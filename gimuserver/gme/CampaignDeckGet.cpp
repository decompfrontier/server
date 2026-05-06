#include "App.hpp"
#include "Handlers.hpp"

// CampaignDeckGet (C3a0VnQK) — returns the campaign-specific party deck
// composition for the deck-select/edit screen.
//
// Response key "w5vHLT0q" — CampaignMissionPartyDeckInfoResponse:
//   [{ "zsiAn9P1": "<DeckNum>",  "edy7fq3L": "<UserUnitId>",
//      "gr48vsdJ": "<MemberType>", "XuJL4pc5": "<Disporder>",
//      "a3qJ6QhX": "<NowHp>",    "3WMz78t6":  "<MaxHp>",
//      "gU2xtQ0V": "<BbGauge>",  "nIGZ1X9C":  "<SbGauge>" }, … ]

struct CampaignDeckMember {
    int32_t deck_num    = 0;
    int32_t user_unit_id = 0;
    int32_t member_type = 0;
    int32_t disporder   = 0;
    int32_t now_hp      = 0;
    int32_t max_hp      = 0;
    int32_t bb_gauge    = 0;
    int32_t sb_gauge    = 0;
};
template <> struct glz::meta<CampaignDeckMember> {
    using T = CampaignDeckMember;
    static constexpr auto value = glz::object(
        "zsiAn9P1", glz::quoted_num<&T::deck_num>,
        "edy7fq3L", glz::quoted_num<&T::user_unit_id>,
        "gr48vsdJ", glz::quoted_num<&T::member_type>,
        "XuJL4pc5", glz::quoted_num<&T::disporder>,
        "a3qJ6QhX", glz::quoted_num<&T::now_hp>,
        "3WMz78t6", glz::quoted_num<&T::max_hp>,
        "gU2xtQ0V", glz::quoted_num<&T::bb_gauge>,
        "nIGZ1X9C", glz::quoted_num<&T::sb_gauge>
    );
};

struct CampaignDeckGetResp {
    std::vector<CampaignDeckMember> members;
};
template <> struct glz::meta<CampaignDeckGetResp> {
    using T = CampaignDeckGetResp;
    static constexpr auto value = glz::object(
        "w5vHLT0q", &T::members
    );
};

HANDLEF(CampaignDeckGet)
{
    LOG_INFO << "CampaignDeckGet: " << json;

    static constexpr std::string_view kUserId = "0839899613932562";

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
                " FROM user_party_decks WHERE user_id=$1 ORDER BY deck_num, disp_order;",
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
