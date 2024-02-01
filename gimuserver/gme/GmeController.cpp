#include "GmeController.hpp"
#include "GmeHandler.hpp"
#include "core/BfCrypt.hpp"
#include "core/Utils.hpp"

constexpr const auto GME_HEADER = "F4q6i9xe";
constexpr const auto GME_BODY = "a3vSYuq2";
constexpr const auto GME_ERROR = "b5PH6mZa";

constexpr const auto HEADER_CLIENT_ID = "aV6cLn3v";
constexpr const auto HEADER_REQUEST_ID = "Hhgi79M1";

constexpr const auto ERROR_ID = "3e9aGpus";
constexpr const auto ERROR_CONTINUE_OP = "iPD12YCr";
constexpr const auto ERROR_MESSAGE = "ZC0msu2L";
constexpr const auto ERROR_UNK_1 = "zcJeTx18";

constexpr const auto BODY_JSON = "Kn51uR4Y";

GmeController::GmeController()
{
	InitializeHandlers();
}

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

	auto header = root[GME_HEADER];

	if (header.isNull())
	{
		LOG_ERROR << "Invalid gme header";
		callback(HttpResponse::newNotFoundResponse()); // TODO: change this to a 500 page error along with DC
		return;
	}

	auto reqid = header[HEADER_REQUEST_ID];

	if (!reqid.isString())
	{
		LOG_ERROR << "Invalid gme request_id";
		callback(HttpResponse::newNotFoundResponse()); // TODO: change this to a 500 page error along with DC
		return;
	}

	std::string encReq = reqid.asCString();

	auto it = m_handlers.find(encReq);

	if (it == m_handlers.end())
	{
		callback(newGmeErrorResponse(encReq, ErrorID::Yes, ErrorOperation::Close, "Unsupported request! Please report this error with the following information:\nRequest:" + encReq));
		return;
	}

	auto q = (it->second)();

	auto body = root[GME_BODY];
	Json::Value bodyJson;

	if (!body.isNull())
	{
		auto enc_json = body[BODY_JSON];
		if (enc_json.isString())
		{
			BfCrypt::DecryptGME(enc_json.asCString(), q->GetAesKey(), bodyJson);

			if (bodyJson.isNull())
			{
				callback(newGmeErrorResponse(encReq, ErrorID::Yes, ErrorOperation::Close, "Unsupported decrypt key! Please report this error with the following information:\nRequest:" + encReq));
				return;
			}
		}
	}
	

	LOG_TRACE << "GME REQUEST " << encReq << " JSON: " << bodyJson.toStyledString();
	
	Json::Value respJson;
	bool handleResult = q->Handle(bodyJson, respJson);

	if (!handleResult)
		callback(newGmeErrorResponse(encReq, q->GetErrorId(), q->GetErrorContinueOp(), q->GetErrorMsg()));
	else
	{
		LOG_TRACE << "GME RESPONSE " << encReq << " JSON: " << respJson.toStyledString();
		callback(newGmeOkResponse(encReq, q->GetAesKey(), respJson));
	}

	delete q;
}

drogon::HttpResponsePtr GmeController::newGmeOkResponse(const std::string& reqId, const std::string& aesKey, const Json::Value& data)
{
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

	return HttpResponse::newHttpJsonResponse(gme);
}

drogon::HttpResponsePtr GmeController::newGmeErrorResponse(const std::string& reqId, ErrorID errId, ErrorOperation errContinueOp, const std::string& msg)
{
	Json::Value error;
	error[ERROR_ID] = std::to_string(static_cast<int>(errId));
	error[ERROR_CONTINUE_OP] = std::to_string(static_cast<int>(errContinueOp));
	error[ERROR_MESSAGE] = msg;
	error[ERROR_UNK_1] = "";

	Json::Value header;
	header[HEADER_CLIENT_ID] = "---";
	header[HEADER_REQUEST_ID] = reqId;

	Json::Value gme;
	gme[GME_HEADER] = header;
	gme[GME_ERROR] = error;

	return HttpResponse::newHttpJsonResponse(gme);
}
