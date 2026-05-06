#include "App.hpp"
#include "Handlers.hpp"

// MissionEnd (9TvyNR5H) — fired by the client after the battle-result screen
// is acknowledged.  Grants fixed rewards, persists them to DB, and returns an
// updated UserTeamInfo so the home-screen HUD reflects the new zel/karma/exp.
//
// Response keys:
//   "fEi17cnx" — [UserTeamInfo]  — refreshes zel / karma / exp in HUD
//   "UT1SVg59" — { "h7eY3sAK": user_id }  — UserClearMissionInfo stub
//
// Fixed rewards (placeholder until real per-mission MST is wired):
//   +500 zel, +100 karma, +100 exp

static constexpr int64_t kMissionZelReward   = 500;
static constexpr int64_t kMissionKarmaReward = 100;
static constexpr int64_t kMissionExpReward   = 100;

// Wraps UserTeamInfo under the "fEi17cnx" single-element array key.
struct MeTeamWrapper {
    UserTeamInfo team_info = {};
};
template<> struct glz::meta<MeTeamWrapper> {
    using T = MeTeamWrapper;
    static constexpr auto value = glz::object(
        "fEi17cnx", pkg::glaze::single_array<&T::team_info>()
    );
};

// ── Build UserTeamInfo from a fresh DB row ────────────────────────────────────
static UserTeamInfo missionEnd_buildTeamInfo(
    const drogon::orm::Row& row,
    const std::vector<UserLevelMst>& prog)
{
    const int32_t level = row["level"].as<int32_t>();
    const UserLevelMst* lv = nullptr;
    for (const auto& e : prog) { if (e.level == level) { lv = &e; break; } }

    UserTeamInfo ti = {};
    ti.user_id              = "0839899613932562";
    ti.level                = level;
    ti.exp                  = row["exp"].as<int64_t>();
    ti.zel                  = row["zel"].as<int64_t>();
    ti.karma                = row["karma"].as<int64_t>();
    ti.brave_coin           = row["brave_coin"].as<int32_t>();
    ti.action_point         = row["energy"].as<int32_t>();
    ti.max_action_point     = lv ? lv->action_points    : 100;
    ti.deck_cost            = lv ? lv->deck_cost        : 20;
    ti.max_friend_count     = lv ? lv->friend_count     : 50;
    ti.add_friend_count     = lv ? lv->add_friend_count : 0;
    ti.max_unit_count       = row["max_unit_count"].as<int32_t>();
    ti.warehouse_count      = row["max_warehouse_count"].as<int32_t>();
    ti.active_deck          = row["active_deck"].as<int32_t>();
    ti.summon_ticket        = row["summon_tickets"].as<int32_t>();
    ti.rainbow_coin         = row["rainbow_coins"].as<int32_t>();
    ti.colosseum_ticket     = row["colosseum_tickets"].as<int32_t>();
    ti.brave_points_total   = row["total_brave_points"].as<int32_t>();
    ti.current_brave_points = row["avail_brave_points"].as<int32_t>();
    ti.want_gift            = row["want_gift"].as<std::string>();
    ti.paid_gems            = row["paid_gems"].as<int32_t>();
    ti.free_gems            = row["free_gems"].as<int32_t>();
    ti.reinforcement_deck.emplace_back(0);
    ti.reinforcement_deck.emplace_back(0);
    ti.reinforcement_deck.emplace_back(0);
    return ti;
}

// ── Handler ──────────────────────────────────────────────────────────────────

HANDLEF(MissionEnd)
{
    LOG_INFO << "MissionEnd: " << json;

    static constexpr std::string_view kUserId = "0839899613932562";

    // Step 1: credit rewards.
    try
    {
        co_await theDb()->execSqlCoro(
            "UPDATE userinfo SET"
            " zel   = MIN(zel   + $2, 99000000),"
            " karma = MIN(karma + $3, 99000000),"
            " exp   = exp + $4"
            " WHERE id=$1;",
            std::string(kUserId),
            kMissionZelReward,
            kMissionKarmaReward,
            kMissionExpReward);
    }
    catch (const drogon::orm::DrogonDbException& ex)
    {
        LOG_WARN << "MissionEnd: reward UPDATE failed: " << ex.base().what();
    }

    // Step 2: fetch fresh userinfo for the response.
    const auto infoRows = co_await theDb()->execSqlCoro(
        "SELECT level, exp, zel, karma, brave_coin, free_gems, paid_gems, energy,"
        " max_unit_count, max_warehouse_count, summon_tickets, rainbow_coins,"
        " colosseum_tickets, total_brave_points, avail_brave_points,"
        " active_deck, want_gift FROM userinfo WHERE id=$1;",
        std::string(kUserId));

    if (infoRows.empty())
    {
        LOG_ERROR << "MissionEnd: userinfo not found";
        co_return HandleResult::success("{}");
    }

    // Step 3: build and serialize fEi17cnx.
    MeTeamWrapper tw{};
    tw.team_info = missionEnd_buildTeamInfo(
        infoRows[0],
        theServer()->cache().initializeResp().progression);

    std::string teamJson{};
    if (const auto ec = glz::write_json(tw, teamJson); ec)
    {
        LOG_ERROR << "MissionEnd: serialize teamInfo: " << glz::format_error(ec, teamJson);
        co_return HandleResult::success("{}");
    }

    // Step 4: append UT1SVg59 (UserClearMissionInfo array) by stripping the
    // trailing '}' from teamJson and appending the extra key.
    if (teamJson.size() >= 2 && teamJson.back() == '}')
    {
        teamJson.pop_back();
        teamJson += R"(,"UT1SVg59":[{"h7eY3sAK":"0839899613932562"}]})";
    }

    co_return HandleResult::success(teamJson);
}
