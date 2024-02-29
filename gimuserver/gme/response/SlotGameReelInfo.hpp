#pragma once

#include "../GmeRequest.hpp"

RESPONSE_NS_BEGIN
struct SlotGameReelInfo : public IResponse
{
	struct Data
	{
		uint32_t id;
		std::string reelData;

		explicit Data() : id(0) {}

		void Serialize(Json::Value& v) const
		{
			v["PINm2pM5"] = std::to_string(id);
			v["Z8eJi4pq"] = reelData;
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
