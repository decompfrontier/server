#pragma once

#include "../GmeRequest.hpp"

REQUEST_NS_BEGIN
struct MstUrlList : public IRequest
{
	const char* getGroupName() const override { return "KeC10fuL"; }

	std::string id;
	std::string version;

protected:
	void DeserializeFields(const Json::Value& v, size_t) override
	{
		id = v.get("moWQ30GH", "").asCString();
		version = v.get("d2RFtP8T", "").asCString();
	}
};
REQUEST_NS_END
