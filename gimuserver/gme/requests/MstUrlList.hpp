#pragma once

#include "../GmeRequest.hpp"

REQUEST_NS_BEGIN
struct MstUrlList : public IRequest
{
	const char* getGroupName() const override { return "KeC10fuL"; }

	void Deserialize(const Json::Value& v) override
	{
		id = v.get("moWQ30GH", "").asCString();
		version = v.get("d2RFtP8T", "").asCString();
	}

	std::string id;
	std::string version;
};
REQUEST_NS_END
