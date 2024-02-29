#pragma once

#include "../GmeRequest.hpp"

RESPONSE_NS_BEGIN
struct MissionBreakInfo : public IResponse
{
	struct Data
	{
		void Serialize(Json::Value& v) const
		{
			// TODO
		}
	};

	const char* getGroupName() const override { return "5PR2VmH1"; }

	std::vector<Data> Mst;

protected:
	size_t getRespCount() const override { return Mst.size(); }

	void SerializeFields(Json::Value& v, size_t i) const override
	{
		Mst.at(i).Serialize(v);
	}
};
RESPONSE_NS_END
