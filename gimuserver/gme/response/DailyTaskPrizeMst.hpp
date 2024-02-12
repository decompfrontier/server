#pragma once

#include "../GmeRequest.hpp"

RESPONSE_NS_BEGIN
struct DailyTaskPrizeMst : public IResponse
{
	struct Data
	{
		explicit Data() : taskId(0), presentType(0),
			targetId(0), targetCount(0), bravePointCost(0), maxClaimCount(0),
			currentClaimCount(0), timeLimit(0), isMileStonePrize(false) {}

		std::string taskTitle, taskDesc, targetParam;
		uint32_t taskId, presentType, targetId, targetCount,
			bravePointCost, maxClaimCount, currentClaimCount;
		uint64_t timeLimit;
		bool isMileStonePrize;

		void Serialize(Json::Value& v) const
		{
			v["d83aQ39U"] = taskId;
			v["T091Rsbe"] = taskTitle;
			v["L2VkgH08"] = taskDesc;
			v["30Kw4WBa"] = presentType;
			v["TdDHf59J"] = targetId;
			v["wJsB35iH"] = targetCount;
			v["37moriMq"] = targetParam;
			v["4NuIwm77"] = bravePointCost;
			v["qY49LBjw"] = timeLimit; // long
			v["D2BlS89M"] = maxClaimCount; // max number of times you can claim this
			v["jT3oB57e"] = currentClaimCount; // if the user has claimed
			v["J3l5We66"] = isMileStonePrize ? "1" : "0";
		}
	};

	const char* getGroupName() const override { return "a739yK18"; }

	std::vector<Data> Mst;

protected:
	size_t getRespCount() const override { return Mst.size(); }

	void SerializeFields(Json::Value& v, size_t i) const override
	{
		Mst.at(i).Serialize(v);
	}
};
RESPONSE_NS_END
