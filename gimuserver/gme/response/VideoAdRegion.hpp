#pragma once

#include "../GmeRequest.hpp"

RESPONSE_NS_BEGIN
struct VideoAdRegion : public IResponse
{
	struct Data
	{
		explicit Data() : id(0) {}

		uint32_t id;
		std::string countryCodes;

		void Serialize(Json::Value& v) const
		{
			v["Y3de0n2p"] = id;
			v["j3d6E2ia"] = countryCodes;
		}
	};

	const char* getGroupName() const override { return "bpD29eiQ"; }

	std::vector<Data> Mst;

protected:
	size_t getRespCount() const override { return Mst.size(); }

	void SerializeFields(Json::Value& v, size_t i) const override
	{
		Mst.at(i).Serialize(v);
	}
};
RESPONSE_NS_END

