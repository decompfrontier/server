#pragma once

#include "../GmeRequest.hpp"

RESPONSE_NS_BEGIN
struct DailyTaskBonusMst : public IResponse
{
	const char* getGroupName() const override { return "p283g07d"; }

	explicit DailyTaskBonusMst() : bonusBravePoints(0) {}

	uint32_t bonusBravePoints;

protected:
	void SerializeFields(Json::Value& v, size_t) const override
	{
		v["k3bD738b"] = std::to_string(bonusBravePoints);
	}
};
RESPONSE_NS_END
