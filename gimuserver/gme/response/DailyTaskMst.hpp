#pragma once

#include "../GmeRequest.hpp"

RESPONSE_NS_BEGIN
struct DailyTaskMst : public IResponse
{
	const char* getGroupName() const override { return "k23D7d43"; }

	struct Data
	{
		std::string typeKey, typeTitle, typeDescription;
		int taskCount, taskBravePoints, bravePointsTotal, bravePoints;
		std::string taskAreaIDs;
		int timesCompleted;

		explicit Data() : taskCount(0), taskBravePoints(0),
			bravePointsTotal(0), bravePoints(0), timesCompleted(0)
		{}

		void Serialize(Json::Value& v) const
		{
			v["O36Qv37k"] = typeKey;
			v["hd2Jf3nC"] = typeTitle;
			v["M7yKr4c1"] = typeDescription;
			v["Y3DbX5ot"] = std::to_string(taskCount);
			v["T4bV8aI9"] = std::to_string(taskBravePoints);
			v["bya9a67k"] = std::to_string(bravePointsTotal);
			v["22rqpZTo"] = std::to_string(bravePoints);
			v["a3011F8b"] = taskAreaIDs;
			v["9cKyb15U"] = timesCompleted;
		}
	};

	std::vector<Data> Mst;

protected:
	size_t getRespCount() const override { return Mst.size(); }

	void SerializeFields(Json::Value& v, size_t i) const override
	{
		Mst.at(i).Serialize(v);
	}
};
RESPONSE_NS_END
