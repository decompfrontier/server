#pragma once

#include <json/value.h>

class DailyTaskConfig
{
	struct DailyPrizeTable
	{
		explicit DailyPrizeTable() : id(0), present_type(0), target_id(0),
			target_count(0), points_cost(0), time_limit(0), max_claims(0),
			milestone_prize(false) {}

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

		explicit DailyTaskBonusTable() : points(0) {}
	};

public:
	void LoadTableFromJson(const std::string& path);
	const auto& GetTaskPrizes() const { return m_prizeTbl; }
	const auto& GetTaskBonusTables() const { return m_taskBonusTbl; }

protected:
	std::vector<DailyPrizeTable> m_prizeTbl;
	std::vector<DailyTaskBonusTable> m_taskBonusTbl;
};
