#include "App.hpp"
#include "Handlers.hpp"

#include <gimuserver/archive/MissionArchiver.hpp>
#include <gimuserver/gme/common/Common.hpp>
#include <chrono>
#include <optional>

// CampaignBattleEnd (pTNB6yw3) — post-battle result handler.
// Marks the mission cleared, credits the cleared mission's zel + karma from the
// mission archive, and returns a fresh UserTeamInfo so the client HUD updates.
//
// Response keys:
//   "fEi17cnx" — [UserTeamInfo]    — refreshes zel / energy in HUD
//   "4MCxgS5p" — CampaignReceiptResponse (stub payload — enough to unblock
//                the receipt screen; real reward logic is future work)
//
// DB writes:
//   1. UPDATE user_campaign_missions SET state=2, clear_count+=1,
//             attain_percent=100, last_cleared_at=<epoch>
//      WHERE user_id=$1 AND mission_id=<active_mission_id>
//   2. UPDATE user_info SET zel = zel + <reward>
//   3. UPDATE user_campaign_state SET active_mission_id=''

// Client overflows zel/karma above this and resets to 0, so cap every credit.
static constexpr int64_t kMaxZelKarma = 99'999'999LL;

// CampaignBattleEndReq (login_info + mission_id) is generated from the KDL
// (packet-generator/assets/net/handlers.kdl).

// ---------------------------------------------------------------------------
// Response: UserTeamInfo (fEi17cnx) + stub receipt (4MCxgS5p)
// ---------------------------------------------------------------------------
struct CampaignBattleEndResp {
    UserTeamInfo team_info = {};
    std::string  receipt_payload = "";  // 4MCxgS5p.pCIRMw04
};
// Two separate keys — we'll hand-assemble the final JSON from both.

// Helper: wrap UserTeamInfo under "fEi17cnx" as a single-element array.
struct CbeBattleEndTeamWrapper {
    UserTeamInfo team_info = {};
};
template <> struct glz::meta<CbeBattleEndTeamWrapper> {
    using T = CbeBattleEndTeamWrapper;
    static constexpr auto value = glz::object(
        "fEi17cnx", pkg::glaze::single_array<&T::team_info>()
    );
};

HANDLEF(CampaignBattleEnd)
{
    LOG_INFO << "CampaignBattleEnd: " << json;

    // Parse — lenient so extra envelope keys don't abort.
    CampaignBattleEndReq req{};
    glz::context ctx{};
    if (const auto ec = glz::read<glz::opts{.error_on_unknown_keys = false}>(req, json, ctx); ec)
    {
        LOG_WARN << "CampaignBattleEnd: parse error: " << glz::format_error(ec, json);
    }

    const auto identity = (co_await gme::getUserIdentity(theDb(), req.login_info)).nonEmpty();
    const std::string kUserId = identity.userId;

    // Read active_mission_id from state table if not in the request body.
    std::string missionId = req.mission_id;
    if (missionId.empty())
    {
        try
        {
            const auto sr = co_await theDb()->execSqlCoro(
                "SELECT active_mission_id FROM user_campaign_state WHERE user_id=$1;",
                std::string(kUserId));
            if (!sr.empty())
                missionId = sr[0]["active_mission_id"].as<std::string>();
        }
        catch (const drogon::orm::DrogonDbException& ex)
        {
            LOG_WARN << "CampaignBattleEnd: state SELECT failed: " << ex.base().what();
        }
    }

    // Rewards are archive-driven: resolve the cleared mission's record up front
    // and fail explicitly when it's missing, rather than crediting a silent
    // default.  Mirrors MissionEnd (9TvyNR5H).
    std::optional<MissionRecord> missionRecord;
    try
    {
        missionRecord = MissionArchiver::instance().lookup(
            static_cast<uint32_t>(std::stoul(missionId)));
    }
    catch (const std::exception&)
    {
        // std::stoul throws on an empty / non-numeric mission id.
    }
    if (!missionRecord)
    {
        co_return HandleResult::error("Archive error",
            "CampaignBattleEnd: no mission archive record for mission '" + missionId + "'");
    }

    // Step 1: mark mission cleared.
    if (!missionId.empty())
    {
        try
        {
            const int64_t now = static_cast<int64_t>(
                std::chrono::duration_cast<std::chrono::seconds>(
                    std::chrono::system_clock::now().time_since_epoch()).count());

            co_await theDb()->execSqlCoro(
                "UPDATE user_campaign_missions"
                " SET state=2, attain_percent=100,"
                "     clear_count = clear_count + 1,"
                "     last_cleared_at = $3"
                " WHERE user_id=$1 AND mission_id=$2;",
                std::string(kUserId), missionId, now);
        }
        catch (const drogon::orm::DrogonDbException& ex)
        {
            LOG_WARN << "CampaignBattleEnd: mission UPDATE failed: " << ex.base().what();
        }

        // Step 1b: unlock the next sequential mission, if any.  This keeps the
        // player on a one-mission-at-a-time progression: only the missions
        // they've earned (cleared one earlier) become available.  Pairs with
        // the PermitPlace mission filter in UserInfo.cpp — the client only
        // sees missions whose row exists in user_campaign_missions, so adding
        // a row here is what makes the next mission visible on the map.
        //
        // INSERT OR IGNORE means we never downgrade a mission that's already
        // available or cleared; we only add brand-new rows.
        try
        {
            const int32_t curId   = std::stoi(missionId);
            const std::string nextId = std::to_string(curId + 1);

            co_await theDb()->execSqlCoro(
                "INSERT OR IGNORE INTO user_campaign_missions"
                " (user_id, mission_id, state, attain_percent)"
                " VALUES ($1, $2, 1, 0);",
                std::string(kUserId), nextId);

            LOG_INFO << "CampaignBattleEnd: cleared mission " << missionId
                     << " — unlocked next mission " << nextId;
        }
        catch (const std::exception& ex)
        {
            // std::stoi throws on non-numeric mission IDs (e.g. event campaigns
            // with alphabetic suffixes).  Skip the unlock step in that case.
            LOG_WARN << "CampaignBattleEnd: next-mission unlock skipped: " << ex.what();
        }
    }

    // Step 2: credit the cleared mission's zel + karma from the archive record.
    try
    {
        co_await theDb()->execSqlCoro(
            "UPDATE user_info"
            " SET zel = MIN(zel + $1, $3),"
            "     karma = MIN(karma + $2, $3)"
            " WHERE id=$4;",
            static_cast<int64_t>(missionRecord->zel),
            static_cast<int64_t>(missionRecord->karma),
            kMaxZelKarma, std::string(kUserId));
    }
    catch (const drogon::orm::DrogonDbException& ex)
    {
        LOG_WARN << "CampaignBattleEnd: reward UPDATE failed: " << ex.base().what();
    }

    // Step 3: clear active mission state.
    try
    {
        co_await theDb()->execSqlCoro(
            "UPDATE user_campaign_state SET active_mission_id='', active_battle_seed=0"
            " WHERE user_id=$1;",
            std::string(kUserId));
    }
    catch (const drogon::orm::DrogonDbException& ex)
    {
        LOG_WARN << "CampaignBattleEnd: state clear failed: " << ex.base().what();
    }

    // Build team_info wrapper + receipt stub, then merge into one JSON object.
    CbeBattleEndTeamWrapper teamWrapper{};
    teamWrapper.team_info = std::move(
        (co_await gme::getTeamInfo(theDb(), identity)).nonEmpty());

    std::string teamJson{};
    if (const auto ec = glz::write_json(teamWrapper, teamJson); ec)
    {
        LOG_ERROR << "CampaignBattleEnd: serialize teamInfo: " << glz::format_error(ec, teamJson);
        co_return HandleResult::success("{}");
    }

    // Inject the receipt key into the response JSON.  We strip the trailing '}'
    // from teamJson and append the receipt field, then close.
    // Receipt payload: {"4MCxgS5p":{"pCIRMw04":""}}
    if (teamJson.size() >= 2 && teamJson.back() == '}')
    {
        teamJson.pop_back();
        teamJson += R"(,"4MCxgS5p":{"pCIRMw04":""}})";
    }

    co_return HandleResult::success(teamJson);
}
