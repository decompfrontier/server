#pragma once

#include "../GmeRequest.hpp"

RESPONSE_NS_BEGIN
struct ExcludedDungeonMissionMst : public IResponse
{
	struct Data
	{
		explicit Data() : id(0) {}

		uint32_t id;

		void Serialize(Json::Value& v) const
		{
			v["j28VNcUW"] = std::to_string(id);
		}
	};

	const char* getGroupName() const override { return "3aDk1xk7"; }

	std::vector<Data> Mst;

protected:
	size_t getRespCount() const override { return Mst.size(); }

	void SerializeFields(Json::Value& v, size_t i) const override
	{
		Mst.at(i).Serialize(v);
	}
};
RESPONSE_NS_END
