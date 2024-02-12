#pragma once

#include "GmeTypes.hpp"
#include "GmeRequest.hpp"
#include "GmeTypes.hpp"
#include <json/value.h>
#include <drogon/HttpResponse.h>

#define HANDLER_NS_BEGIN namespace Handler {
#define HANDLER_NS_END }

HANDLER_NS_BEGIN
class HandlerBase;
using DrogonCallback = std::function<void(const drogon::HttpResponsePtr&)>;
using HandlerCallback = void(*)(HandlerBase* p);

class HandlerBase
{
public:
	explicit HandlerBase(std::string reqId, DrogonCallback&& drogon_cb) : m_errID(ErrorID::No), m_errOP(ErrorOperation::Close), m_drogonCb(drogon_cb), m_reqId(reqId) {}

	virtual const char* GetAesKey() const = 0;
	virtual void Handle(const Json::Value& req) = 0;

protected:
	void FinishHandling(const Json::Value& res = Json::Value());

	std::string m_errMsg;
	ErrorID m_errID;
	ErrorOperation m_errOP;
	DrogonCallback m_drogonCb;
	std::string m_reqId;
};
HANDLER_NS_END

drogon::HttpResponsePtr newGmeErrorResponse(const std::string& reqId, ErrorID errId, ErrorOperation errContinueOp, const std::string& msg);
drogon::HttpResponsePtr newGmeOkResponse(const std::string& reqId, const std::string& aesKey, const Json::Value& data);
