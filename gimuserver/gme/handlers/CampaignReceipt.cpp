#include "App.hpp"
#include "Handlers.hpp"

#include <gimuserver/gme/common/Common.hpp>

// CampaignReceipt (5Imq3wC0) — claims the reward for a cleared campaign mission.
// Marks the mission as reward_claimed in the DB, credits a fixed zel reward,
// and returns:
//   "fEi17cnx" — [UserTeamInfo]          — refreshes HUD zel/karma/exp
//   "4MCxgS5p" — { "pCIRMw04": "" }      — receipt payload (stub)

static constexpr int64_t kReceiptZelReward   = 1000;
static constexpr int64_t kReceiptKarmaReward = 200;

struct CampaignReceiptReq {
    std::string mission_id = "";  // j28VNcUW
};
template <> struct glz::meta<CampaignReceiptReq> {
    using T = CampaignReceiptReq;
    static constexpr auto value = glz::object(
        "j28VNcUW", &T::mission_id
    );
};

// Wraps UserTeamInfo under the "fEi17cnx" single-element array key.
struct CrTeamWrapper {
    UserTeamInfo team_info = {};
};
template<> struct glz::meta<CrTeamWrapper> {
    using T = CrTeamWrapper;
    static constexpr auto value = glz::object(
        "fEi17cnx", pkg::glaze::single_array<&T::team_info>()
    );
};

static UserTeamInfo campaignReceipt_buildTeamInfo(
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
    ti.friend_point         = row["friend_points"].as<int32_t>();
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

HANDLEF(CampaignReceipt)
{
    LOG_INFO << "CampaignReceipt: " << json;

    // Transitional bridge: resolve the sole offline user at runtime
    // (tutorial-created).  TODO port to gme::getUserIdentity.
    const std::string kUserId = co_await gme::getSoleUserId(theDb());

    CampaignReceiptReq req{};
    {
        glz::context ctx{};
        if (const auto ec = glz::read<glz::opts{.error_on_unknown_keys = false}>(req, json, ctx); ec)
            LOG_WARN << "CampaignReceipt: parse error: " << glz::format_error(ec, json);
    }

    // Mark reward as claimed so the mission tile stops showing the receipt badge.
    if (!req.mission_id.empty())
    {
        try
        {
            co_await theDb()->execSqlCoro(
                "UPDATE user_campaign_missions SET reward_claimed=1"
                " WHERE user_id=$1 AND mission_id=$2;",
                std::string(kUserId), req.mission_id);
        }
        catch (const drogon::orm::DrogonDbException& ex)
        {
            LOG_WARN << "CampaignReceipt: mission UPDATE failed: " << ex.base().what();
        }
    }

    // Credit receipt reward.
    try
    {
        co_await theDb()->execSqlCoro(
            "UPDATE userinfo SET"
            " zel   = MIN(zel   + $2, 99999999),"
            " karma = MIN(karma + $3, 99999999)"
            " WHERE id=$1;",
            std::string(kUserId), kReceiptZelReward, kReceiptKarmaReward);
    }
    catch (const drogon::orm::DrogonDbException& ex)
    {
        LOG_WARN << "CampaignReceipt: reward UPDATE failed: " << ex.base().what();
    }

    // Fetch fresh userinfo and build fEi17cnx so the HUD updates.
    const auto infoRows = co_await theDb()->execSqlCoro(
        "SELECT level, exp, zel, karma, brave_coin, 0 AS free_gems, gems AS paid_gems, energy,"
        " max_unit_count, max_warehouse_count, summon_tickets, rainbow_coins,"
        " colosseum_tickets, friend_points, total_brave_points, avail_brave_points,"
        " active_deck, want_gift FROM userinfo WHERE id=$1;",
        std::string(kUserId));

    if (infoRows.empty())
    {
        LOG_WARN << "CampaignReceipt: userinfo not found — returning stub";
        co_return HandleResult::success(R"({"4MCxgS5p":{"pCIRMw04":""}})");
    }

    CrTeamWrapper tw{};
    tw.team_info = campaignReceipt_buildTeamInfo(
        infoRows[0],
        theServer()->cache().initializeResp().progression);

    std::string teamJson{};
    if (const auto ec = glz::write_json(tw, teamJson); ec)
    {
        LOG_WARN << "CampaignReceipt: serialize failed — returning stub";
        co_return HandleResult::success(R"({"4MCxgS5p":{"pCIRMw04":""}})");
    }

    // Append receipt key inside the same root object.
    if (teamJson.size() >= 2 && teamJson.back() == '}')
    {
        teamJson.pop_back();
        teamJson += R"(,"4MCxgS5p":{"pCIRMw04":""}})";
    }

    co_return HandleResult::success(teamJson);
}
