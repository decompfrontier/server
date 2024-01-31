#include "GmeController.hpp"
#include "GmeCoder.hpp"
#include "GmeHandler.hpp"
#include "core/BfCrypt.hpp"
#include "core/Utils.hpp"

void GmeController::HandleGame(const HttpRequestPtr& rq, std::function<void(const HttpResponsePtr&)>&& callback)
{
	//Utils::DumpInfoToDrogon(rq, "GmeController");

	Json::Reader r;
	Json::Value root;

	auto err = r.parse(rq->getBody().data(), root);
	if (!err)
	{
		LOG_ERROR << "Cannot decode gme request: " << r.getFormatedErrorMessages();
		callback(HttpResponse::newNotFoundResponse()); // TODO: change this to a 500 page error along with DC
		return;
	}

	auto header = root[JENC("header")];

	if (header.isNull())
	{
		LOG_ERROR << "Invalid gme header";
		callback(HttpResponse::newNotFoundResponse()); // TODO: change this to a 500 page error along with DC
		return;
	}

	auto reqid = header[JENC("request_id")];

	if (!reqid.isString())
	{
		LOG_ERROR << "Invalid gme request_id";
		callback(HttpResponse::newNotFoundResponse()); // TODO: change this to a 500 page error along with DC
		return;
	}

	std::string encReq = reqid.asCString();
	auto decReq = GmeCoder::Instance().GetRequestNameFromEncName(encReq);

	if (decReq.empty())
	{
		callback(newGmeErrorResponse(encReq, ErrorID::Yes, ErrorOperation::Close, "Unsupported request! Please report this error with the following information:\nRequest:" + encReq));
		return;
	}

	auto body = root[JENC("body")];
	Json::Value bodyJson;

	if (!body.isNull())
	{
		auto enc_json = body[JENC("encrypted_json")];
		if (enc_json.isString())
		{
			bodyJson = BfCrypt::DecryptGME(enc_json.asCString(), GmeCoder::Instance().GetAESKeyByDecName(decReq));
		}
	}
	

	LOG_TRACE << "GME REQUEST " << decReq << " JSON: " << bodyJson.toStyledString();
	
	if (!m_handler.Handle(decReq, bodyJson))
	{
		callback(newGmeErrorResponse(encReq, m_handler.GetErrorId(), m_handler.GetErrorContinueOp(), m_handler.GetErrorMsg()));
		return;
	}

	callback(newGmeOkResponse(decReq, m_handler.GetSuccessData()));
}

drogon::HttpResponsePtr GmeController::newGmeOkResponse(const std::string& reqId, const Json::Value& data)
{

	Json::Value header;
	header[JENC("client_id")] = "---";
	header[JENC("request_id")] = GmeCoder::Instance().GetFromDecName(reqId);
	header[JENC("?_0")] = "{}";
	header[JENC("?_1")] = 1;

	Json::Value gme;
	gme[JENC("header")] = header;

	if (!data.isNull())
	{ // sometimes it can be null
		Json::Value body;
		body[JENC("encrypted_json")] = data; // TODO: CRYPT THE DATA
		gme[JENC("body")] = BfCrypt::CryptGME(body, GmeCoder::Instance().GetAESKeyByDecName(reqId));
	}

	return HttpResponse::newHttpJsonResponse(gme);
}

drogon::HttpResponsePtr GmeController::newGmeErrorResponse(const std::string& reqId, ErrorID errId, ErrorOperation errContinueOp, const std::string& msg)
{
	Json::Value error;
	error[JENC("error_id")] = std::to_string(static_cast<int>(errId));
	error[JENC("error_continue_operation")] = std::to_string(static_cast<int>(errContinueOp));
	error[JENC("error_message")] = msg;
	error[JENC("?_2")] = "";

	Json::Value header;
	header[JENC("client_id")] = "---";
	header[JENC("request_id")] = reqId;
	header[JENC("?_0")] = "{}";
	header[JENC("?_1")] = 1;

	Json::Value gme;
	gme[JENC("header")] = header;
	gme[JENC("error")] = error;

	return HttpResponse::newHttpJsonResponse(gme);
}
