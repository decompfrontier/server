#pragma once

#include "../GmeRequest.hpp"

RESPONSE_NS_BEGIN
struct DailyLoginRewardsUserInfo : public IResponse
{
	const char* getGroupName() const override { return "Drudr2w5"; }
	bool isArray() const override { return false; }

	explicit DailyLoginRewardsUserInfo() :
		id(1), userCurrentCount(0), userLimitCount(0),
		currentDay(1), guaranteedRemainigDays(0), nextRewardId(0)
	{
		message = " day(s) more to guaranteed Gem!";
	}

	uint32_t id;
	uint32_t userCurrentCount;
	uint32_t userLimitCount;
	uint32_t currentDay;
	uint32_t guaranteedRemainigDays;
	std::string message;
	uint32_t nextRewardId;

protected:
	void SerializeFields(Json::Value& v, size_t) const override
	{
		v["XIvaD6Jp"] = std::to_string(id);
		v["35JXN4Ay"] = std::to_string(userCurrentCount);
		v["5xStG99s"] = std::to_string(userLimitCount);
		v["ad6i23pO"] = std::to_string(currentDay);
		v["u8iD6ka7"] = std::to_string(guaranteedRemainigDays);
		v["ZC0msu2L"] = message;

		if (nextRewardId > 0)
			v["outas79f"] = "";
		else
			v["outas79f"] = std::to_string(nextRewardId);
	}
};
RESPONSE_NS_END
