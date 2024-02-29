#pragma once

#include "../GmeRequest.hpp"

RESPONSE_NS_BEGIN
struct VersionInfo : public IResponse
{
	struct Data
	{
		explicit Data() : Version(0), Unknown(0), SubVersion(0) {}

		std::string Id;
		unsigned int Version;
		unsigned int Unknown;
		unsigned int SubVersion;

		void Serialize(Json::Value& v) const
		{
			v["moWQ30GH"] = Id;
			//v["e3QNsuZ8"] = Description;
			v["d2RFtP8T"] = std::to_string(Version);
			v["H6k1LIxC"] = std::to_string(Unknown);
			v["5kbnkTp0"] = std::to_string(SubVersion);
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
