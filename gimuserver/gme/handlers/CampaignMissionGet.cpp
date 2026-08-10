#include "App.hpp"
#include "Handlers.hpp"

#include <gimuserver/gme/common/Common.hpp>

// CampaignMissionGet (RSm6p2d4) — returns per-mission progress/state.
// Response is the CampaignMissionInfoResponse shape (group "2I9V0o6J"):
//   [{ "j28VNcUW": <MissionID>,
//      "HUo4T7i8": "<AttainPercent>",
//      "JcKMjH64": "<MissionOnFlg>",
//      "j0Uszek2": "<State>" }, …]
// CampaignMissionEntry + CampaignMissionGetResp are generated from the KDL
// (packet-generator/assets/net/handlers.kdl).  CampaignMissionEntry is shared
// with CampaignStart.

HANDLEF(CampaignMissionGet)
{
    LOG_INFO << "CampaignMissionGet: " << json;

    CampaignMissionGetReq req{};
    {
        glz::context ctx{};
        if (const auto ec = glz::read<glz::opts{.error_on_unknown_keys = false}>(req, json, ctx); ec)
            LOG_WARN << "CampaignMissionGet: parse error: " << glz::format_error(ec, json);
    }

    const auto identity = (co_await gme::getUserIdentity(theDb(), req.login_info)).nonEmpty();
    const std::string kUserId = identity.userId;

    CampaignMissionGetResp resp{};

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
        LOG_WARN << "CampaignMissionGet: SELECT failed: " << ex.base().what();
        co_return HandleResult::success("{}");
    }

    co_return HandleResult::success(glz::write_json(resp).value_or("{}"));
}
