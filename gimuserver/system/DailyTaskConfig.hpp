#pragma once

#include <json/value.h>

class DailyTaskConfig
{
	struct DailyPrizeTable
	{
		int id;
		std::string title;
		std::string desc;
		int present_type;
		int target_id;
		int target_count;
		std::string target_param;
		int points_cost;
		long long time_limit;
		int max_claims;
		bool milestone_prize;
	};

	struct DailyTaskBonusTable
	{
		int points;
	};

public:
	bool LoadTableFromJson(const std::string& path);
	const auto& GetTaskPrizes() const { return m_prizeTbl; }
	const auto& GetTaskBonusTables() const { return m_taskBonusTbl; }

protected:
	std::vector<DailyPrizeTable> m_prizeTbl;
	std::vector<DailyTaskBonusTable> m_taskBonusTbl;
};
