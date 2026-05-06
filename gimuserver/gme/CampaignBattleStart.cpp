#include "App.hpp"
#include "Handlers.hpp"

// CampaignBattleStart (h1RjcD3S) — pre-battle request fired when the player
// confirms their deck and taps "Battle".  Persists the active mission ID so
// CampaignBattleEnd knows which mission row to update.  The client renders the
// battle entirely from its local stage/MST data; the server response only needs
// to not close the session (empty {} is sufficient for now — flesh out once a
// real capture is available).
//
// Key request fields (inferred from createBody pattern):
//   "IKqx1Cn9"[0]["B5JQyV8j"] — handle name (createUserInfoTag)
//   Mission ID is likely in a top-level field; we log the full body so we can
//   identify the exact key from the first real capture.

struct CampaignBattleStartReq {
    std::string mission_id = "";  // actual key TBD from capture
};
template <> struct glz::meta<CampaignBattleStartReq> {
    using T = CampaignBattleStartReq;
    // Minimal struct — we only need lenient parsing to avoid the silent abort
    // caused by the IKqx1Cn9 envelope key.  Real field keys will be added once
    // a captured request confirms them.
    static constexpr auto value = glz::object(
        "j28VNcUW", &T::mission_id   // j28VNcUW = MissionID (reused across Campaign)
    );
};

HANDLEF(CampaignBattleStart)
{
    LOG_INFO << "CampaignBattleStart: " << json;

    static constexpr std::string_view kUserId = "0839899613932562";

    CampaignBattleStartReq req{};
    {
        glz::context ctx{};
        // Lenient parse — request carries IKqx1Cn9 + other unknown envelope keys.
        if (const auto ec = glz::read<glz::opts{.error_on_unknown_keys = false}>(req, json, ctx); ec)
        {
            LOG_WARN << "CampaignBattleStart: parse error: " << glz::format_error(ec, json);
        }
    }

    // Persist active mission so BattleEnd can update the right row.
    if (!req.mission_id.empty())
    {
        try
        {
            co_await theDb()->execSqlCoro(
                "INSERT INTO user_campaign_state (user_id, active_mission_id)"
                " VALUES ($1,$2)"
                " ON CONFLICT(user_id) DO UPDATE SET active_mission_id=$2;",
                std::string(kUserId), req.mission_id);
        }
        catch (const drogon::orm::DrogonDbException& ex)
        {
            LOG_WARN << "CampaignBattleStart: state UPSERT failed: " << ex.base().what();
        }
    }

    co_return HandleResult::success("{}");
}
