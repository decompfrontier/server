#pragma once

#include "../GmeRequest.hpp"

RESPONSE_NS_BEGIN
struct VideoAdsSlotGameStandInfo : public IResponse
{
	uint32_t ads_count, max_ads_count,
		current_bonus, max_bonus_count,
		ads_bonus_flag, next_day_timer;

	explicit VideoAdsSlotGameStandInfo() : ads_count(0),
		max_ads_count(0), current_bonus(0), max_bonus_count(0),
		ads_bonus_flag(0), next_day_timer(0) {}

	const char* getGroupName() const override { return nullptr; }
	bool isArray() const override { return false; }

protected:
	void SerializeFields(Json::Value& v, size_t i) const override
	{
		v["wRIgGCHh"] = std::to_string(ads_count);
		v["JwBrVzIZ"] = std::to_string(max_ads_count);
		v["BrMgnCaT"] = std::to_string(current_bonus);
		v["E9gMeBW0"] = std::to_string(max_bonus_count);
		v["qqdr4HlW"] = std::to_string(ads_bonus_flag);
		v["er8Ups6U"] = std::to_string(next_day_timer);
	}
};
RESPONSE_NS_END
