#include "App.hpp"
#include "Handlers.hpp"

// CampaignMissionGet (RSm6p2d4) — returns per-mission progress/state.
// Response is the CampaignMissionInfoResponse shape (group "2I9V0o6J"):
//   [{ "j28VNcUW": <MissionID>,
//      "HUo4T7i8": "<AttainPercent>",
//      "JcKMjH64": "<MissionOnFlg>",
//      "j0Uszek2": "<State>" }, …]
struct CampaignMissionEntry {
    std::string mission_id;
    int32_t     attain_percent = 0;
    std::string mission_on_flg = "1";
    int32_t     state = 1;
};
template <> struct glz::meta<CampaignMissionEntry> {
    using T = CampaignMissionEntry;
    static constexpr auto value = glz::object(
        "j28VNcUW", &T::mission_id,
        "HUo4T7i8", glz::quoted_num<&T::attain_percent>,
        "JcKMjH64", &T::mission_on_flg,
        "j0Uszek2", glz::quoted_num<&T::state>
    );
};

struct CampaignMissionGetResp {
    std::vector<CampaignMissionEntry> missions;
};
template <> struct glz::meta<CampaignMissionGetResp> {
    using T = CampaignMissionGetResp;
    static constexpr auto value = glz::object(
        "2I9V0o6J", &T::missions
    );
};

HANDLEF(CampaignMissionGet)
{
    LOG_INFO << "CampaignMissionGet: " << json;

    static constexpr std::string_view kUserId = "0839899613932562";

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
