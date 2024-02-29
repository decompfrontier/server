#pragma once

#include "../GmeRequest.hpp"

RESPONSE_NS_BEGIN
struct GuildInfo : public IResponse
{
	const char* getGroupName() const override { return "IkdSufj5"; }
	bool isArray() const override { return false; }

	explicit GuildInfo() :
		id(0)
	{}

	uint32_t id;

protected:
	void SerializeFields(Json::Value& v, size_t) const override
	{
		v["sD73jd20"] = id;

		if (id != 0)
		{
			// TODO: fill up this structure
		}
	}
};
RESPONSE_NS_END
