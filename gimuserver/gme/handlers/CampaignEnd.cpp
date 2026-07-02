#include "App.hpp"
#include "Handlers.hpp"

#include <gimuserver/gme/common/Common.hpp>

// CampaignEnd (jF9Kkro4) — fired when the player exits the Campaign menu.
// Clears any in-flight battle state.  Empty {} response keeps the session
// alive (mirrors EventTokenInfo / TownUpdate stubs).
HANDLEF(CampaignEnd)
{
    LOG_INFO << "CampaignEnd: " << json;

    // Transitional bridge: resolve the sole offline user at runtime
    // (tutorial-created).  TODO port to gme::getUserIdentity.
    const std::string kUserId = co_await gme::getSoleUserId(theDb());

    try
    {
        co_await theDb()->execSqlCoro(
            "UPDATE user_campaign_state SET active_mission_id='', active_battle_seed=0"
            " WHERE user_id=$1;",
            std::string(kUserId));
    }
    catch (const drogon::orm::DrogonDbException& ex)
    {
        LOG_WARN << "CampaignEnd: state clear failed: " << ex.base().what();
    }

    co_return HandleResult::success("{}");
}
