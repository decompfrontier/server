#pragma once

#include "../GmeRequest.hpp"

RESPONSE_NS_BEGIN
struct DailyTaskBonusMst : public IResponse
{
	const char* getGroupName() const override { return "p283g07d"; }

	struct Data
	{
		explicit Data() : bonusBravePoints(0) {}

		uint32_t bonusBravePoints;

		void Serialize(Json::Value& v) const
		{
			v["k3bD738b"] = std::to_string(bonusBravePoints);
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
