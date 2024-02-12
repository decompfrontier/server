#pragma once

#include "../GmeRequest.hpp"

RESPONSE_NS_BEGIN
struct DailyTaskPrizeMst : public IResponse
{
	const char* getGroupName() const override { return "a739yK18"; }

	explicit DailyTaskPrizeMst() : taskId(0), presentType(0),
		targetId(0), targetCount(0), bravePointCost(0), maxClaimCount(0),
		currentClaimCount(0), timeLimit(0), isMileStonePrize(false) {}

	std::string taskTitle, taskDesc, targetParam;
	uint32_t taskId, presentType, targetId, targetCount,
		bravePointCost, maxClaimCount, currentClaimCount;
	uint64_t timeLimit;
	bool isMileStonePrize;

protected:
	void SerializeFields(Json::Value& v, size_t) const override
	{
		v["d83aQ39U"] = std::to_string(taskId);
		v["T091Rsbe"] = taskTitle;
		v["L2VkgH08"] = taskDesc;
		v["30Kw4WBa"] = std::to_string(presentType);
		v["TdDHf59J"] = std::to_string(targetId);
		v["wJsB35iH"] = std::to_string(targetCount);
		v["37moriMq"] = targetParam;
		v["4NuIwm77"] = std::to_string(bravePointCost);
		v["qY49LBjw"] = std::to_string(timeLimit); // long
		v["D2BlS89M"] = std::to_string(maxClaimCount);
		v["jT3oB57e"] = std::to_string(currentClaimCount);
		v["J3l5We66"] = isMileStonePrize ? "1" : "0";
	}
};
RESPONSE_NS_END
