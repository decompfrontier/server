#pragma once

#include "../GmeRequest.hpp"

RESPONSE_NS_BEGIN
struct BadgeInfo : public IResponse
{
	const char* getGroupName() const override { return "h23iRjGN"; }

	explicit BadgeInfo() {}

protected:
	void SerializeFields(Json::Value& v, size_t) const override
	{
	}
};
RESPONSE_NS_END
