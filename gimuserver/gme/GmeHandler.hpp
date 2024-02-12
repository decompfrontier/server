#pragma once

#include "GmeTypes.hpp"
#include "GmeRequest.hpp"
#include "GmeTypes.hpp"
#include <json/value.h>
#include <drogon/HttpResponse.h>
#include <drogon/Session.h>

#define HANDLER_NS_BEGIN namespace Handler {
#define HANDLER_NS_END }

HANDLER_NS_BEGIN
class HandlerBase;
using DrogonCallback = std::function<void(const drogon::HttpResponsePtr&)>;

class HandlerBase
{
public:
	virtual const char* GetGroupId() const = 0;
	virtual const char* GetAesKey() const = 0;
	virtual void Handle(const drogon::SessionPtr& session, DrogonCallback& cb, const Json::Value& req) const = 0;

protected:
	void FinishHandling(const drogon::SessionPtr& session, DrogonCallback& cb, const Json::Value& res = Json::Value()) const;
};
HANDLER_NS_END

drogon::HttpResponsePtr newGmeErrorResponse(const std::string& reqId, ErrorID errId, ErrorOperation errContinueOp, const std::string& msg);
drogon::HttpResponsePtr newGmeOkResponse(const std::string& reqId, const std::string& aesKey, const Json::Value& data);
