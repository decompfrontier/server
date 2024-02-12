#pragma once

#include "../GmeRequest.hpp"

RESPONSE_NS_BEGIN
struct LoginCampaignReward : public IResponse
{
	const char* getGroupName() const override { return "bD18x9Ti"; }

	struct Data
	{
		explicit Data() : id(1), reward_day(0) {}

		uint32_t id;
		uint32_t reward_day;
		std::string reward_img;

		void Serialize(Json::Value& v) const
		{
			v["H1Dkq93v"] = std::to_string(id);
			v["n0He37p1"] = reward_day;
			v["b38adb8i"] = reward_img;
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
