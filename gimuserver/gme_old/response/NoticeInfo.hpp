#pragma once

#include "../GmeRequest.hpp"

RESPONSE_NS_BEGIN
struct NoticeInfo : public IResponse
{
	const char* getGroupName() const override { return "Pj6zDW3m"; }
	bool isArray() const override { return false; }

	uint32_t id;
	std::string url;

	explicit NoticeInfo() : id(1) {}

protected:
	void SerializeFields(Json::Value& v, size_t) const override
	{
		v["xJNom6i0"] = std::to_string(id);
		v["jsRoN50z"] = url;
	}
};
RESPONSE_NS_END
