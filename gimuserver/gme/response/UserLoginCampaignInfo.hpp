#pragma once

#include "../GmeRequest.hpp"

RESPONSE_NS_BEGIN
struct UserLoginCampaignInfo : public IResponse
{
	const char* getGroupName() const override { return "3da6bd0a"; }

	uint32_t id;
	uint32_t currentDay;
	uint32_t totalDays;
	bool firstForTheDay;

	explicit UserLoginCampaignInfo()
		: id(1), currentDay(0), totalDays(0), firstForTheDay(false)
	{}


protected:
	void SerializeFields(Json::Value& v, size_t) const override
	{
		v["H1Dkq93v"] = std::to_string(id);
		v["ad6i23pO"] = currentDay;
		v["1adb38d5"] = totalDays;
		v["4tswNoV9"] = firstForTheDay ? 1 : 0;
	}
};
RESPONSE_NS_END