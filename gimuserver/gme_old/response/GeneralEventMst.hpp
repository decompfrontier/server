#pragma once

#include "../GmeRequest.hpp"

RESPONSE_NS_BEGIN
struct GeneralEventMst : public IResponse
{
	struct Data
	{
		void Serialize(Json::Value& v) const
		{
			// TODO
		}
	};

	const char* getGroupName() const override { return "Md9gG3c0"; }

	std::vector<Data> Mst;

protected:
	size_t getRespCount() const override { return Mst.size(); }

	void SerializeFields(Json::Value& v, size_t i) const override
	{
		Mst.at(i).Serialize(v);
	}
};
RESPONSE_NS_END
