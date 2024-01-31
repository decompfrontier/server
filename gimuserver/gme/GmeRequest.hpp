#pragma once

#include <string>
#include <json/value.h>

#define REQUEST_NS_BEGIN namespace Request {
#define REQUEST_NS_END }
#define RESPONSE_NS_BEGIN namespace Response {
#define RESPONSE_NS_END }


REQUEST_NS_BEGIN
class IRequest
{
	virtual const char* getGroupName() const = 0;
	virtual void Deserialize(const Json::Value& v) = 0;
};
REQUEST_NS_END

RESPONSE_NS_BEGIN
class IResponse
{
	virtual const char* getGroupName() const = 0;
	virtual void Serialize(Json::Value& v) const = 0;
};
RESPONSE_NS_END
