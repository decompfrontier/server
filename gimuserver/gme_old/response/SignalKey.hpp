#pragma once

#include "../GmeRequest.hpp"

RESPONSE_NS_BEGIN
struct SignalKey : public IResponse
{
	const char* getGroupName() const override { return "6FrKacq7"; }

	std::string key;

protected:
	void SerializeFields(Json::Value& v, size_t) const override
	{
		v["Kn51uR4Y"] = key;
	}
};
RESPONSE_NS_END
