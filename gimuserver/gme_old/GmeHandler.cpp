#include "GmeHandler.hpp"
#include "GmeTypes.hpp"
#include <core/Utils.hpp>
#include <core/BfCrypt.hpp>
#include <drogon/HttpResponse.h>

drogon::HttpResponsePtr newGmeOkResponse(const std::string& reqId, const std::string& aesKey, const Json::Value& data)
{
	Utils::AppendJsonResToFile(data, reqId);

	Json::Value header;
	header[HEADER_CLIENT_ID] = "---";
	header[HEADER_REQUEST_ID] = reqId;

	Json::Value gme;
	gme[GME_HEADER] = header;

	if (!data.isNull())
	{
		// sometimes it can be null
		Json::Value body;
		body[BODY_JSON] = BfCrypt::CryptGME(data, aesKey);
		gme[GME_BODY] = body;
	}

	return drogon::HttpResponse::newHttpJsonResponse(gme);
}

drogon::HttpResponsePtr newGmeErrorResponse(const std::string& reqId, ErrorID errId, ErrorOperation errContinueOp, const std::string& msg)
{
	Json::Value error;
	error[ERROR_ID] = static_cast<int>(errId);
	error[ERROR_CONTINUE_OP] = static_cast<int>(errContinueOp);
	error[ERROR_MESSAGE] = msg;
	error[ERROR_UNK_1] = "";

	Json::Value header;
	header[HEADER_CLIENT_ID] = "---";
	header[HEADER_REQUEST_ID] = reqId;

	Json::Value gme;
	gme[GME_HEADER] = header;
	gme[GME_ERROR] = error;

	return drogon::HttpResponse::newHttpJsonResponse(gme);
}

void Handler::HandlerBase::OnError(const drogon::orm::DrogonDbException& e, DrogonCallback cb, ErrorOperation op) const
{
	cb(newGmeErrorResponse(GetGroupId(), ErrorID::Yes, op, e.base().what()));
}
