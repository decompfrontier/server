#pragma once

#include "../GmeRequest.hpp"

RESPONSE_NS_BEGIN
struct TownLocationMst : public IResponse
{
	const char* getGroupName() const override { return "1y2JDv79"; }

	struct Data
	{
		explicit Data() : id(0), pos_x(0), pos_y(0), width(0),
			height(0), effect_type(0), need_mission_id(0) {}

		uint32_t id, pos_x, pos_y, width, height,
			effect_type, need_mission_id;
		std::string name;

		void Serialize(Json::Value& v) const
		{
			v["un80kW9Y"] = id;
			v["6ntp4rMV"] = name; // str
			v["SnNtTh51"] = pos_x;
			v["M6C1aXfR"] = pos_y;
			v["dRhvW13q"] = width;
			v["FCzW4g6P"] = height;
			v["jeR2rN3V"] = effect_type;
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

