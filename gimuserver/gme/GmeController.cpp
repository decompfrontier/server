#include "GmeController.hpp"
#include "GmeHandler.hpp"
#include "GmeTypes.hpp"
#include "core/BfCrypt.hpp"
#include "core/Utils.hpp"
#include "core/System.hpp"
#include "gme/response/FeatureCheck.hpp"
#include <db/DbMacro.hpp>

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
        callback(HttpResponse::newNotFoundResponse());
        return;
    }

    auto header = root[GME_HEADER];
    if (header.isNull())
    {
        LOG_ERROR << "Invalid gme header";
        callback(HttpResponse::newNotFoundResponse());
        return;
    }

    auto reqid = header[HEADER_REQUEST_ID];
    if (!reqid.isString())
    {
        LOG_ERROR << "Invalid gme request_id";
        callback(HttpResponse::newNotFoundResponse());
        return;
    }

    std::string encReq = reqid.asCString();

    auto it = m_handlers.find(encReq);
    if (it == m_handlers.end())
    {
        callback(newGmeErrorResponse(encReq, ErrorID::Yes, ErrorOperation::Close,
            "Unsupported request! Please report this error with the following information:\nRequest:" + encReq));
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
                callback(newGmeErrorResponse(encReq, ErrorID::Yes, ErrorOperation::Close,
                    "Unsupported decrypt key! Please report this error with the following information:\nRequest:" + encReq));
                return;
            }
        }
    }

    Utils::AppendJsonReqToFile(bodyJson, q.GetGroupId());

    // Fast path — user already resolved from a previous request or startup.
    const std::string& cachedID = System::Instance().GetActiveUserID();
    if (!cachedID.empty())
    {
        DispatchToUser(cachedID, q, callback, bodyJson);
        return;
    }

    // Slow path — fresh install: DB was empty at startup so ResolveActiveUser
    // found nothing. HandleGuest has since created the account; query it now,
    // cache it, and dispatch. All subsequent requests take the fast path.
    LOG_INFO << "GmeController: active user not cached, resolving from DB for request " << encReq;

    GME_DB->execSqlAsync(
        "SELECT id FROM users LIMIT 1;",
        [this, &q, callback, bodyJson](const drogon::orm::Result& result)
        {
            if (result.empty())
            {
                // HandleGuest hasn't finished yet — client should retry.
                LOG_WARN << "GmeController: no user in DB yet during lazy resolve.";
                callback(newGmeErrorResponse(q.GetGroupId(), ErrorID::Yes, ErrorOperation::Close,
                    "Account is still being created, please try again in a moment."));
                return;
            }

            const std::string resolvedID = result[0]["id"].as<std::string>();
            LOG_INFO << "GmeController: lazily resolved user id=" << resolvedID;
            System::Instance().SetActiveUserID(resolvedID);
            DispatchToUser(resolvedID, q, callback, bodyJson);
        },
        [&q, callback](const drogon::orm::DrogonDbException& e)
        {
            LOG_ERROR << "GmeController: DB error during lazy user resolve: " << e.base().what();
            callback(newGmeErrorResponse(q.GetGroupId(), ErrorID::Yes, ErrorOperation::Close,
                "Internal server error during user resolution."));
        }
    );
}

void GmeController::DispatchToUser(const std::string& userId,
    const Handler::HandlerBase& handler,
    Handler::DrogonCallback callback,
    const Json::Value& bodyJson)
{
    if (m_users.find(userId) == m_users.end())
    {
        UserInfo ui;
        ui.info.userID = userId;
        m_users.insert_or_assign(userId, ui);
    }
    handler.Handle(m_users[userId], callback, bodyJson);
}

void GmeController::HandleFeatureCheck(const HttpRequestPtr& rq, std::function<void(const HttpResponsePtr&)>&& callback)
{
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