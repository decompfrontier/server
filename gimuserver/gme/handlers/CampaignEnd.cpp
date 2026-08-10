#include "App.hpp"
#include "Handlers.hpp"

#include <gimuserver/gme/common/Common.hpp>

// CampaignEnd (jF9Kkro4) — fired when the player exits the Campaign menu.
// Clears any in-flight battle state.  Empty {} response keeps the session
// alive (mirrors EventTokenInfo / TownUpdate stubs).
HANDLEF(CampaignEnd)
{
    LOG_INFO << "CampaignEnd: " << json;

    CampaignEndReq req{};
    {
        glz::context ctx{};
        if (const auto ec = glz::read<glz::opts{.error_on_unknown_keys = false}>(req, json, ctx); ec)
            LOG_WARN << "CampaignEnd: parse error: " << glz::format_error(ec, json);
    }

    const auto identity = (co_await gme::getUserIdentity(theDb(), req.login_info)).nonEmpty();
    const std::string kUserId = identity.userId;

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
