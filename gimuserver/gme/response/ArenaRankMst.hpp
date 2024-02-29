#pragma once

#include "../GmeRequest.hpp"

RESPONSE_NS_BEGIN
struct ArenaRankMst : public IResponse
{
	const char* getGroupName() const override { return "6kWq78zx"; }

	struct Data
	{
		explicit Data() : id(0), rank_point_end(0), rank_point_start(0), reward_type(0) {}

		uint32_t id, rank_point_start, rank_point_end, reward_type;
		std::string name, reward_param, scenario_info;

		void Serialize(Json::Value& v) const
		{
			v["JmFn3g9t"] = std::to_string(id);
			v["rGm09bav"] = name;
			v["w0aTd94Y"] = std::to_string(rank_point_start);
			v["1U3eBCyY"] = std::to_string(rank_point_end); // int
			v["IkmC8gG2"] = std::to_string(reward_type);
			v["empaR60j"] = reward_param; // str
			v["N4XVE1uA"] = scenario_info;
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
