#pragma once

#include "../GmeRequest.hpp"

RESPONSE_NS_BEGIN
struct TownFacilityMst : public IResponse
{
	const char* getGroupName() const override { return "Lh1I3dGo"; }

	struct Data
	{
		explicit Data() : id(0), pos_x(0), pos_y(0), width(0),
			height(0), effect_type(0), need_mission_id(0) {}

		uint32_t id, pos_x, pos_y, width, height,
			effect_type, need_mission_id;
		std::string name;

		void Serialize(Json::Value& v) const
		{
			v["y9ET7Aub"] = id;
			v["aAFI6S5w"] = name; // str
			v["SnNtTh51"] = pos_x;
			v["M6C1aXfR"] = pos_y;
			v["dRhvW13q"] = width;
			v["FCzW4g6P"] = height;
			v["HSRhkf70"] = need_mission_id;
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
