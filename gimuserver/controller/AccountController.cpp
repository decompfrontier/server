#include "App.hpp"
#include "AccountController.hpp"
#include <gimuserver/packets/net_gumi_live.hpp>

using namespace drogon;
using namespace drogon::orm;

Task<> AccountController::HandleGuest(HttpRequestPtr rq, std::function<void(const HttpResponsePtr&)> callback)
{
    DumpLog log;
    theServer()->tryOpenHttpDumpLog("guestLogin", log);
    log << rq << "\n";

    // Extract parameters
    const auto& params = rq->getParameters();
    GuestLogin login;
    login.status = v_StatusEnum::Error;
    login.status_number = 1; // TODO: what was this again?

    try
    {
        // Note: something real should be used here if you want multi-user
        login.user_id = "00000001";

        try
        {
            // Check if user exists in users table
            const auto& result = co_await theDb()->execSqlCoro("SELECT id FROM gumi_live_users WHERE id = $1", login.user_id);
            if (result.empty()) {
                /*
                    How should it work in an actual system that uses an external API for login?

                    1. Game client tries to login or make a new account "GuestLogin"
                    2. Based from ak and other identifiers a userid and token is computed
                    3. Token is passed back to the game
                    4. The game will try to initialize with the gumi user token
                    5. Once the token is validated, the initialize request should make a new user

                    Here there is only the user creation step.
                */

                // NOTE: This should be de-hardcoded if someone plans to make a multiuser or multiprofile system.

                // User does not exist, add him to the table
                co_await theDb()->execSqlCoro("INSERT INTO gumi_live_users(id) VALUES ($1);", login.user_id);
                LOG_DEBUG << "AccountController: new user " << login.user_id;
            }

            login.status = v_StatusEnum::Success;
            login.token = "test_token"; // Note: Should we use a proper token with drogon session?
            login.status_number = 0;

        }
        catch (drogon::orm::DrogonDbException ex)
        {
            LOG_ERROR << "AccountController: Query execution failed: " << ex.base().what();
        }
    }
    catch (std::invalid_argument ex)
    {
        LOG_DEBUG << "AccountController: invalid parameters " << ex.what();
    }

    auto resp = HttpResponse::newHttpResponse();
    resp->setCloseConnection(true); // we live the session torugh the gme controllers from the token...

    std::string output{};
    auto ec = glz::write_json(login, output);
    if (ec)
    {
        LOG_ERROR << "AccountController: Cannot serialize GuestLogin " << ec;
        resp->setStatusCode(k500InternalServerError);
        log << "Serialization error!";
    }
    else
    {
        resp->setStatusCode(k200OK);
        resp->setContentTypeCode(ContentType::CT_APPLICATION_JSON);
        resp->setBody(output);
        log << output;
    }

    callback(resp);
}

