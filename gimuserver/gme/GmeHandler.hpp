#pragma once

#include "GmeTypes.hpp"
#include "GmeRequest.hpp"
#include <json/value.h>

#define HANDLER_NS_BEGIN namespace Handler {
#define HANDLER_NS_END }

HANDLER_NS_BEGIN
class IHandler
{
public:
	auto GetErrorId() const { return m_errID; }
	auto GetErrorContinueOp() const { return m_errOP; }
	const auto& GetErrorMsg() const { return m_errMsg; }

	virtual const char* GetAesKey() const = 0;
	virtual bool Handle(const Json::Value& req, Json::Value& res) = 0;

protected:
	std::string m_errMsg;
	ErrorID m_errID;
	ErrorOperation m_errOP;	
};
HANDLER_NS_END
