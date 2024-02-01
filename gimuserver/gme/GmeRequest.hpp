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
public:
	virtual const char* getGroupName() const = 0;

	virtual void Deserialize(const Json::Value& v)
	{
		if (!v.isArray())
			return;

		for (size_t i = 0; i < v.size(); i++)
		{
			Json::Value g;
			DeserializeFields(g, i);
		}
	}

protected:
	virtual void DeserializeFields(const Json::Value& v, size_t pos) = 0;
};
REQUEST_NS_END

RESPONSE_NS_BEGIN
class IResponse
{
public:
	virtual const char* getGroupName() const = 0;

	virtual void Serialize(Json::Value& v) const
	{
		for (size_t i = 0; i < getRespCount(); i++)
		{
			Json::Value g;
			SerializeFields(g, i);
			v[getGroupName()].insert(i, g);
		}
	}

protected:
	virtual size_t getRespCount() const { return 1; }
	virtual void SerializeFields(Json::Value& v, size_t pos) const = 0;
};
RESPONSE_NS_END
