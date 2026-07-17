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
            "UPDATE user_info SET"
            " zel   = MIN(zel   + $2, 99999999),"
            " karma = MIN(karma + $3, 99999999)"
            " WHERE id=$1;",
            std::string(kUserId), kReceiptZelReward, kReceiptKarmaReward);
    }
    catch (const drogon::orm::DrogonDbException& ex)
    {
        LOG_WARN << "CampaignReceipt: reward UPDATE failed: " << ex.base().what();
    }

    // Fetch fresh user_info and build fEi17cnx so the HUD updates.
    CrTeamWrapper tw{};
    tw.team_info = std::move(
        (co_await gme::getTeamInfo(theDb(),
            gme::UserIdentity{.userId = std::string(kUserId)})).nonEmpty());

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
