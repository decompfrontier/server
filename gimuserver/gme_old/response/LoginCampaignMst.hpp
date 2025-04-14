#pragma once


#include "../GmeRequest.hpp"

RESPONSE_NS_BEGIN
struct LoginCampaignMst : public IResponse
{
	const char* getGroupName() const override { return "Bd29Pqw0"; }

	explicit LoginCampaignMst() :
		id(1), startDate(0), totalDays(0)
	{}

	uint32_t id;
	uint32_t startDate;
	uint32_t totalDays;
	std::string image;

protected:
	void SerializeFields(Json::Value& v, size_t) const override
	{
		v["H1Dkq93v"] = std::to_string(id);
		v["qA7M9EjP"] = startDate;
		v["1adb38d5"] = std::to_string(totalDays);
		v["b38adb8i"] = image;
	}
};
RESPONSE_NS_END
