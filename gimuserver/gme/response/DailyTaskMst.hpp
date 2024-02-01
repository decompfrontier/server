#pragma once

#include "../GmeRequest.hpp"

RESPONSE_NS_BEGIN
struct DailyTaskMst : public IResponse
{
	const char* getGroupName() const override { return "k23D7d43"; }

	std::string typeKey, typeTitle, typeDescription;
	int taskCount, taskBravePoints, bravePointsTotal, bravePoints;
	std::string taskAreaIDs;
	int timesCompleted;

protected:
	void SerializeFields(Json::Value& v, size_t) const override
	{
		v["O36Qv37k"] = typeKey;
		v["hd2Jf3nC"] = typeTitle;
		v["M7yKr4c1"] = typeDescription;
		v["Y3DbX5ot"] = std::to_string(taskCount); // str->int
		v["T4bV8aI9"] = std::to_string(taskBravePoints); // str->int
		v["bya9a67k"] = std::to_string(bravePointsTotal); // str->int
		v["22rqpZTo"] = std::to_string(bravePoints); // str->int
		v["a3011F8b"] = taskAreaIDs;
		v["9cKyb15U"] = std::to_string(timesCompleted); // str->int
	}

};
RESPONSE_NS_END
