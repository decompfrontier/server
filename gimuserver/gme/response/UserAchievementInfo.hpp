#pragma once

#include "../GmeRequest.hpp"

RESPONSE_NS_BEGIN
struct UserAchievementInfo : public IResponse
{
	bool isArray() const override { return false; }
	const char* getGroupName() const override { return "Bnc4LpM8"; }

protected:
	void SerializeFields(Json::Value& v, size_t i) const override
	{
		// TODO
	}
};
RESPONSE_NS_END
