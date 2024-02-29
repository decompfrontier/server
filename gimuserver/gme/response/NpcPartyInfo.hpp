#pragma once

#include "../GmeRequest.hpp"

RESPONSE_NS_BEGIN
struct NpcPartyInfo : public IResponse
{
	struct Data
	{
		uint32_t id;
		uint32_t type;
		uint32_t disp_order;

		explicit Data() : id(0), type(0), disp_order(0) {}

		void Serialize(Json::Value& v) const
		{
			v["P_USER_UNIT_ID"] = std::to_string(id);
			v["P_MEMBER_TYPE"] = std::to_string(type);
			v["P_DISPORDER"] = std::to_string(disp_order);
		}
	};

	const char* getGroupName() const override { return nullptr; }

	std::vector<Data> Mst;

protected:
	size_t getRespCount() const override { return Mst.size(); }

	void SerializeFields(Json::Value& v, size_t i) const override
	{
		Mst.at(i).Serialize(v);
	}
};
RESPONSE_NS_END
