#include "App.hpp"
#include "GmeController.hpp"

#include <gimuserver/utils/BfCrypt.hpp>
#include <gimuserver/packets/net_gme.hpp>

using namespace drogon;

Task<> GmeController::HandleGame(HttpRequestPtr rq, std::function<void(const HttpResponsePtr&)> callback)
{
	auto resp = HttpResponse::newHttpResponse();
	auto req = glz::read_json<GmeAction>(rq->getBody());
	if (req || !req.has_value())
	{
		LOG_ERROR << "Cannot decode gme request: " << req.error();
		resp->setCloseConnection(true);
		resp->setStatusCode(k400BadRequest);
	}
	else
	{
		auto& gmeReq = req.value();
		if (!gmeReq.body.has_value() || gmeReq.header.id.empty() || gmeReq.error.has_value())
		{
			resp->setCloseConnection(true);
			resp->setStatusCode(k400BadRequest);
		}
		else
		{
			const auto& gmeResp = co_await Handle(rq->session(), gmeReq);
			if (gmeResp.error.has_value())
			{
				auto cmd = gmeResp.error.value().cmd;
				resp->setCloseConnection(cmd == GmeErrorCommand::Close || cmd == GmeErrorCommand::Close2 || cmd == GmeErrorCommand::Close3 ||
					cmd == GmeErrorCommand::LogoutFacebook || cmd == GmeErrorCommand::RaidError);
			}

			std::string buffer{};
			auto ec = glz::write_json(gmeResp, buffer);
			if (ec)
			{
				LOG_ERROR << "Cannot create gme response: " << req.value().body.value().body;
				resp->setCloseConnection(true); // kill connection in case of an error
				resp->setStatusCode(k500InternalServerError);
			}
			else
			{
				resp->setStatusCode(k200OK);
				resp->setBody(buffer);
			}
		}
	}

	callback(resp);

#if 0
	// NOTE: in a real server this shouldn't happen
	if (m_users.empty())
	{
		UserInfo ui;
		ui.info.userID = HARDCODE_USERID;
		m_users.insert_or_assign(ui.info.userID, ui);
	}
	auto& user = m_users[HARDCODE_USERID];
	
	q.Handle(user, callback, bodyJson);
#endif
}

void GmeController::HandleFeatureCheck(const HttpRequestPtr& rq, std::function<void(const HttpResponsePtr&)>&& callback)
{
	auto p = HttpResponse::newHttpResponse();
	p->setStatusCode(k200OK);
	p->setContentTypeCode(CT_APPLICATION_JSON);
	p->setBody(theServer()->cache().feature());
	callback(p);
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
	// TODO: Decompose this!!
	p->setBody("W3siUFJFU0VOVF9UWVBFIjoiMiIsIlBSRVNFTlRfSUQiOiIwIiwiUFJFU0VOVF9DTlQiOiIyMDAiLCJSRVdBUkQiOmZhbHNlLCJHUk9VUF9UWVBFIjoiMSIsIkRBWVNfTEVGVF9UT19HRU0iOjR9LHsiUFJFU0VOVF9UWVBFIjoiMyIsIlBSRVNFTlRfSUQiOiIwIiwiUFJFU0VOVF9DTlQiOiIxMDAwMCIsIlJFV0FSRCI6ZmFsc2UsIkdST1VQX1RZUEUiOiIxIiwiREFZU19MRUZUX1RPX0dFTSI6NH0seyJQUkVTRU5UX1RZUEUiOiI2IiwiUFJFU0VOVF9JRCI6IjEwMjAzIiwiUFJFU0VOVF9DTlQiOiIxIiwiUkVXQVJEIjpmYWxzZSwiR1JPVVBfVFlQRSI6IjEiLCJEQVlTX0xFRlRfVE9fR0VNIjo0fSx7IlBSRVNFTlRfVFlQRSI6IjYiLCJQUkVTRU5UX0lEIjoiMjAyMDMiLCJQUkVTRU5UX0NOVCI6IjEiLCJSRVdBUkQiOmZhbHNlLCJHUk9VUF9UWVBFIjoiMSIsIkRBWVNfTEVGVF9UT19HRU0iOjR9LHsiUFJFU0VOVF9UWVBFIjoiNiIsIlBSRVNFTlRfSUQiOiIzMDIwMyIsIlBSRVNFTlRfQ05UIjoiMSIsIlJFV0FSRCI6ZmFsc2UsIkdST1VQX1RZUEUiOiIxIiwiREFZU19MRUZUX1RPX0dFTSI6NH0seyJQUkVTRU5UX1RZUEUiOiI2IiwiUFJFU0VOVF9JRCI6IjQwMjAzIiwiUFJFU0VOVF9DTlQiOiIxIiwiUkVXQVJEIjpmYWxzZSwiR1JPVVBfVFlQRSI6IjEiLCJEQVlTX0xFRlRfVE9fR0VNIjo0fSx7IlBSRVNFTlRfVFlQRSI6IjYiLCJQUkVTRU5UX0lEIjoiNTAyMDMiLCJQUkVTRU5UX0NOVCI6IjEiLCJSRVdBUkQiOnRydWUsIkdST1VQX1RZUEUiOiIxIiwiREFZU19MRUZUX1RPX0dFTSI6NH0seyJQUkVTRU5UX1RZUEUiOiI2IiwiUFJFU0VOVF9JRCI6IjYwMTMzIiwiUFJFU0VOVF9DTlQiOiIxIiwiUkVXQVJEIjpmYWxzZSwiR1JPVVBfVFlQRSI6IjEiLCJEQVlTX0xFRlRfVE9fR0VNIjo0fSx7IlBSRVNFTlRfVFlQRSI6IjUiLCJQUkVTRU5UX0lEIjoiMjI0MDAiLCJQUkVTRU5UX0NOVCI6IjEiLCJSRVdBUkQiOmZhbHNlLCJHUk9VUF9UWVBFIjoiMSIsIkRBWVNfTEVGVF9UT19HRU0iOjR9XQ==");
	callback(p);
}
