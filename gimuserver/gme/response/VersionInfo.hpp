#pragma once

#include "../GmeRequest.hpp"

RESPONSE_NS_BEGIN
struct VersionInfo : public IResponse
{
	struct Data
	{
		std::string Id;
		unsigned int Version;
		unsigned int Unknown;
		unsigned int SubVersion;

		void Serialize(Json::Value& v) const
		{
			v["moWQ30GH"] = Id;
			//v["e3QNsuZ8"] = Description;
			v["d2RFtP8T"] = Version;
			v["H6k1LIxC"] = Unknown;
			v["5kbnkTp0"] = SubVersion;
		}
	};

	std::vector<Data> Mst;

	const char* getGroupName() const override { return "KeC10fuL"; }

protected:
	size_t getRespCount() const override { return Mst.size(); }

	void SerializeFields(Json::Value& v, size_t i) const override
	{
		Mst.at(i).Serialize(v);
	}
};
RESPONSE_NS_END
