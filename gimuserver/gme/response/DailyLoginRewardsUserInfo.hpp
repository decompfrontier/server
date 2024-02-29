#pragma once

#include "../GmeRequest.hpp"

RESPONSE_NS_BEGIN
struct DailyLoginRewardsUserInfo : public IResponse
{
	const char* getGroupName() const override { return "Drudr2w5"; }
	bool isArray() const override { return false; }

	explicit DailyLoginRewardsUserInfo() :
		id(1), unk(0), unk2(0),
		currentDay(1), guaranteedRemainigDays(0), unk6(0)
	{
		remainingDaysMsg = " day(s) more to guaranteed Gem!";
	}

	uint32_t id;
	uint32_t unk;
	uint32_t unk2;
	uint32_t currentDay;
	uint32_t guaranteedRemainigDays;
	std::string remainingDaysMsg;
	uint32_t unk6;

protected:
	void SerializeFields(Json::Value& v, size_t) const override
	{
		// needs to do full decomp to understand this...

		v["XIvaD6Jp"] = id;
		v["35JXN4Ay"] = unk;
		v["5xStG99s"] = unk2;
		v["ad6i23pO"] = currentDay;
		v["u8iD6ka7"] = std::to_string(guaranteedRemainigDays);
		v["ZC0msu2L"] = remainingDaysMsg;
		v["outas79f"] = unk6;
	}
};
RESPONSE_NS_END
