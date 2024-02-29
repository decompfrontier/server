#pragma once

#include "../GmeRequest.hpp"

RESPONSE_NS_BEGIN
struct SummonTicketV2Mst : public IResponse
{
	struct Data
	{
		void Serialize(Json::Value& v) const
		{
			// TODO
		}
	};

	const char* getGroupName() const override { return "hE1d083b"; }

	std::vector<Data> Mst;

protected:
	size_t getRespCount() const override { return Mst.size(); }

	void SerializeFields(Json::Value& v, size_t i) const override
	{
		Mst.at(i).Serialize(v);
	}
};
RESPONSE_NS_END
