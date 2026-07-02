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

// ---------------------------------------------------------------------------
// Mission entry (2I9V0o6J array element)
// ---------------------------------------------------------------------------
struct CsStartMissionEntry {
    std::string mission_id;
    int32_t     attain_percent = 0;
    std::string mission_on_flg = "1";
    int32_t     state = 1;
};
template <> struct glz::meta<CsStartMissionEntry> {
    using T = CsStartMissionEntry;
    static constexpr auto value = glz::object(
        "j28VNcUW", &T::mission_id,
        "HUo4T7i8", glz::quoted_num<&T::attain_percent>,
        "JcKMjH64", &T::mission_on_flg,
        "j0Uszek2", glz::quoted_num<&T::state>
    );
};

// ---------------------------------------------------------------------------
// Party deck entry (hT95cq8K array element)
// ---------------------------------------------------------------------------
struct CsStartDeckEntry {
    int32_t     deck_num    = 0;
    std::string guest_id    = "";
    int32_t     member_type = 0;
};
template <> struct glz::meta<CsStartDeckEntry> {
    using T = CsStartDeckEntry;
    static constexpr auto value = glz::object(
        "zsiAn9P1", glz::quoted_num<&T::deck_num>,
        "7zyHb5h9", &T::guest_id,
        "gr48vsdJ", glz::quoted_num<&T::member_type>
    );
};

// ---------------------------------------------------------------------------
// Mission deck info entry (Yusr3Zg5 array element)
// ---------------------------------------------------------------------------
struct CsStartMissionDeckEntry {
    int32_t deck_num      = 0;
    int32_t now_point_num = 0;
};
template <> struct glz::meta<CsStartMissionDeckEntry> {
    using T = CsStartMissionDeckEntry;
    static constexpr auto value = glz::object(
        "zsiAn9P1", glz::quoted_num<&T::deck_num>,
        "7w0inC1R", glz::quoted_num<&T::now_point_num>
    );
};

// ---------------------------------------------------------------------------
// Event info object (RseDpY04 — single object, not array)
// ---------------------------------------------------------------------------
struct CsStartEventInfo {
    int32_t reserve_party_disp_flg = 0;
};
template <> struct glz::meta<CsStartEventInfo> {
    using T = CsStartEventInfo;
    static constexpr auto value = glz::object(
        "W1EkCyc3", glz::quoted_num<&T::reserve_party_disp_flg>
    );
};

// Empty-array placeholder for item slots (NjZ6ds1S, 5EByfWJ4, p04iC2wr).
// These are always empty in Phase 1; a dedicated type avoids the glz::json_t
// dependency while still serializing as [].
struct CsEmptyEntry { int32_t unused = 0; };
template <> struct glz::meta<CsEmptyEntry> {
    using T = CsEmptyEntry;
    static constexpr auto value = glz::object("_", &T::unused);
};

// ---------------------------------------------------------------------------
// Full response body
// ---------------------------------------------------------------------------
struct CampaignStartResp {
    std::vector<CsStartMissionEntry>    missions;       // 2I9V0o6J
    std::vector<CsStartDeckEntry>       party_decks;    // hT95cq8K
    std::vector<CsEmptyEntry>           eqp_items;      // NjZ6ds1S (empty)
    std::vector<CsEmptyEntry>           rsv_items;      // 5EByfWJ4 (empty)
    std::vector<CsStartMissionDeckEntry> mission_decks; // Yusr3Zg5
    CsStartEventInfo                    event_info = {}; // RseDpY04
    std::vector<CsEmptyEntry>           reward_bonus;   // p04iC2wr (empty)
};
template <> struct glz::meta<CampaignStartResp> {
    using T = CampaignStartResp;
    static constexpr auto value = glz::object(
        "2I9V0o6J", &T::missions,
        "hT95cq8K", &T::party_decks,
        "NjZ6ds1S", &T::eqp_items,
        "5EByfWJ4", &T::rsv_items,
        "Yusr3Zg5", &T::mission_decks,
        "RseDpY04", &T::event_info,
        "p04iC2wr", &T::reward_bonus
    );
};

// ---------------------------------------------------------------------------
HANDLEF(CampaignStart)
{
    LOG_INFO << "CampaignStart: " << json;

    // Transitional bridge: resolve the sole offline user at runtime
    // (tutorial-created).  TODO port to gme::getUserIdentity.
    const std::string kUserId = co_await gme::getSoleUserId(theDb());

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
            CsStartMissionEntry e{};
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
            CsStartDeckEntry e{};
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
        CsStartMissionDeckEntry md{};
        md.deck_num      = i;
        md.now_point_num = 0;
        resp.mission_decks.emplace_back(md);
    }

    co_return HandleResult::success(glz::write_json(resp).value_or("{}"));
}
