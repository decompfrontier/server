#pragma once

#include "../GmeRequest.hpp"

RESPONSE_NS_BEGIN
struct UserPurchaseInfo : public IResponse
{
	const char* getGroupName() const override { return "4W6EhXLS"; }
	bool isArray() const override { return false; }

protected:
	void SerializeFields(Json::Value& v, size_t i) const override
	{

	}
};
RESPONSE_NS_END
