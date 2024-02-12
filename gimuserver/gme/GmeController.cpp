#include "GmeController.hpp"
#include "GmeHandler.hpp"
#include "GmeTypes.hpp"
#include "core/BfCrypt.hpp"
#include "core/Utils.hpp"

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

	const auto& q = *it->second;

	auto body = root[GME_BODY];
	Json::Value bodyJson;

	if (!body.isNull())
	{
		auto enc_json = body[BODY_JSON];
		if (enc_json.isString())
		{
			BfCrypt::DecryptGME(enc_json.asCString(), q.GetAesKey(), bodyJson);

			if (bodyJson.isNull())
			{
				callback(newGmeErrorResponse(encReq, ErrorID::Yes, ErrorOperation::Close, "Unsupported decrypt key! Please report this error with the following information:\nRequest:" + encReq));
				return;
			}
		}
	}

	LOG_TRACE << "GME REQUEST " << encReq << " JSON: " << bodyJson.toStyledString();


	// NOTE: in a real server this shouldn't happen
	if (m_users.empty())
	{
		UserInfo ui;
		ui.info.userID = HARDCODE_USERID;
		m_users.insert_or_assign(ui.info.userID, ui);
	}
	auto& user = m_users[HARDCODE_USERID];
	
	q.Handle(user, callback, bodyJson);
}

void GmeController::HandleFeatureCheck(const HttpRequestPtr& rq, std::function<void(const HttpResponsePtr&)>&& callback)
{
	callback(
		HttpResponse::newNotFoundResponse() // TODO
	);
}

void GmeController::HandleServerTime(const HttpRequestPtr& rq, std::function<void(const HttpResponsePtr&)>&& callback)
{
	callback(
		HttpResponse::newNotFoundResponse() // TODO
	);
		
		//trantor::Date::date().secondsSinceEpoch();
}
