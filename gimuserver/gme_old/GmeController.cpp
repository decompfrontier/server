#include "GmeController.hpp"
#include "GmeHandler.hpp"
#include "GmeTypes.hpp"
#include "core/BfCrypt.hpp"
#include "core/Utils.hpp"
#include "gme/response/FeatureCheck.hpp"

GmeController::GmeController()
{
	InitializeHandlers();
}

void GmeController::HandleGame(const HttpRequestPtr& rq, std::function<void(const HttpResponsePtr&)>&& callback)
{
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

	Utils::AppendJsonReqToFile(bodyJson, q.GetGroupId());

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
	// note: drogon should already cache this
	Json::Value v;
	Response::FeatureCheck c;
	c.Serialize(v);
	callback(HttpResponse::newHttpJsonResponse(v));
}

void GmeController::HandleServerTime(const HttpRequestPtr& rq, std::function<void(const HttpResponsePtr&)>&& callback)
{
	auto p = HttpResponse::newHttpResponse();
	p->setStatusCode(k200OK);
	p->setContentTypeCode(CT_TEXT_HTML);
	p->setBody(std::to_string(trantor::Date::date().secondsSinceEpoch()));
	callback(p);
}

void GmeController::HandleDailyLogin(const HttpRequestPtr& rq, std::function<void(const HttpResponsePtr&)>&& callback)
{
	auto p = HttpResponse::newHttpResponse();
	p->setStatusCode(k200OK);
	p->setContentTypeCode(CT_APPLICATION_JSON);
	p->setBody("W3siUFJFU0VOVF9UWVBFIjoiMiIsIlBSRVNFTlRfSUQiOiIwIiwiUFJFU0VOVF9DTlQiOiIyMDAiLCJSRVdBUkQiOmZhbHNlLCJHUk9VUF9UWVBFIjoiMSIsIkRBWVNfTEVGVF9UT19HRU0iOjR9LHsiUFJFU0VOVF9UWVBFIjoiMyIsIlBSRVNFTlRfSUQiOiIwIiwiUFJFU0VOVF9DTlQiOiIxMDAwMCIsIlJFV0FSRCI6ZmFsc2UsIkdST1VQX1RZUEUiOiIxIiwiREFZU19MRUZUX1RPX0dFTSI6NH0seyJQUkVTRU5UX1RZUEUiOiI2IiwiUFJFU0VOVF9JRCI6IjEwMjAzIiwiUFJFU0VOVF9DTlQiOiIxIiwiUkVXQVJEIjpmYWxzZSwiR1JPVVBfVFlQRSI6IjEiLCJEQVlTX0xFRlRfVE9fR0VNIjo0fSx7IlBSRVNFTlRfVFlQRSI6IjYiLCJQUkVTRU5UX0lEIjoiMjAyMDMiLCJQUkVTRU5UX0NOVCI6IjEiLCJSRVdBUkQiOmZhbHNlLCJHUk9VUF9UWVBFIjoiMSIsIkRBWVNfTEVGVF9UT19HRU0iOjR9LHsiUFJFU0VOVF9UWVBFIjoiNiIsIlBSRVNFTlRfSUQiOiIzMDIwMyIsIlBSRVNFTlRfQ05UIjoiMSIsIlJFV0FSRCI6ZmFsc2UsIkdST1VQX1RZUEUiOiIxIiwiREFZU19MRUZUX1RPX0dFTSI6NH0seyJQUkVTRU5UX1RZUEUiOiI2IiwiUFJFU0VOVF9JRCI6IjQwMjAzIiwiUFJFU0VOVF9DTlQiOiIxIiwiUkVXQVJEIjpmYWxzZSwiR1JPVVBfVFlQRSI6IjEiLCJEQVlTX0xFRlRfVE9fR0VNIjo0fSx7IlBSRVNFTlRfVFlQRSI6IjYiLCJQUkVTRU5UX0lEIjoiNTAyMDMiLCJQUkVTRU5UX0NOVCI6IjEiLCJSRVdBUkQiOnRydWUsIkdST1VQX1RZUEUiOiIxIiwiREFZU19MRUZUX1RPX0dFTSI6NH0seyJQUkVTRU5UX1RZUEUiOiI2IiwiUFJFU0VOVF9JRCI6IjYwMTMzIiwiUFJFU0VOVF9DTlQiOiIxIiwiUkVXQVJEIjpmYWxzZSwiR1JPVVBfVFlQRSI6IjEiLCJEQVlTX0xFRlRfVE9fR0VNIjo0fSx7IlBSRVNFTlRfVFlQRSI6IjUiLCJQUkVTRU5UX0lEIjoiMjI0MDAiLCJQUkVTRU5UX0NOVCI6IjEiLCJSRVdBUkQiOmZhbHNlLCJHUk9VUF9UWVBFIjoiMSIsIkRBWVNfTEVGVF9UT19HRU0iOjR9XQ==");
	callback(p);
}
