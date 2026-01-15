#include "DailyTaskConfig.hpp"
#include <json/json.h>
#include <fstream>

void DailyTaskConfig::LoadTableFromJson(const std::string& path)
{
	std::ifstream ifs;
	ifs.open(path + "/dailytask.json");

	if (!ifs.is_open())
		throw std::runtime_error("dailytask.json: not found");

	Json::CharReaderBuilder rb;
	JSONCPP_STRING jss;
	Json::Value root;
	if (!Json::parseFromStream(rb, ifs, &root, &jss))
		throw std::runtime_error("dailytask.json: parse error");

	const auto& msts = root["task_prizes"];

	int id = 1;
	for (const auto& v : msts)
	{
		DailyTaskConfig::DailyPrizeTable tbl;

		auto sid = v.get("start_id", Json::Value());
		if (sid.isNumeric())
			id = sid.asInt();

		tbl.id = id;
		tbl.title = v["task_title"].asCString();
		tbl.desc = v["task_description"].asCString();
		tbl.present_type = v["present_type"].asInt();
		tbl.target_id = v["target_id"].asInt();
		tbl.target_param = v["target_param"].asCString();
		tbl.time_limit = v["time_limit"].asInt64();
		tbl.max_claims = v["max_claim_count"].asInt();
		tbl.points_cost = v["points_cost"].asInt();
		tbl.target_count = v["target_count"].asInt();
		tbl.milestone_prize = v["milestone_prize"].asBool();

		m_prizeTbl.emplace_back(tbl);
		id++;
	}

	const auto& mst2 = root["task_bonus"];

	for (const auto& v : mst2)
	{
		DailyTaskConfig::DailyTaskBonusTable tbl2;

		tbl2.points = v["points"].asInt();

		m_taskBonusTbl.emplace_back(tbl2);
	}
}
