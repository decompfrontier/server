#pragma once

#include "../GmeRequest.hpp"

RESPONSE_NS_BEGIN
struct VideoAdInfo : public IResponse
{
	struct Data
	{
		explicit Data() : id(0), availableValue(0), regionId(0),
			videoEnabled(false), nextAvailableTimeLeft(0)
		{}

		uint32_t id;
		uint32_t availableValue;
		uint32_t regionId;
		bool videoEnabled;
		uint64_t nextAvailableTimeLeft;

		void Serialize(Json::Value& v) const
		{
			v["k3ab6D82"] = std::to_string(id);
			v["Diwl3b56"] = availableValue;
			v["Y3de0n2p"] = std::to_string(regionId);
			v["26adZ1iy"] = videoEnabled ? 1 : 0;
			v["oohpPLCt"] = nextAvailableTimeLeft;
		}
	};

	const char* getGroupName() const override { return "j129kD6r"; }

	std::vector<Data> Mst;

protected:
	size_t getRespCount() const override { return Mst.size(); }

	void SerializeFields(Json::Value& v, size_t i) const override
	{
		Mst.at(i).Serialize(v);
	}
};
RESPONSE_NS_END
