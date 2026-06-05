#include "App.hpp"
#include "AccountController.hpp"

#include <gimuserver/db/UserInfoService.hpp>
#include <gimuserver/gme/common.hpp>
#include <gimuserver/utils/Random.hpp>

#include <stdexcept>

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
    login.status = StatusEnum::Error;
    login.status_number = 1; // TODO: what was this again?

    try
    {
        // For the login flow, this is the first server endpoint the client hits.
        // The client sends device/login identifiers such as vid, altvid, ak, and
        // identifiers, but not a local Gumi Live user ID. This local server currently
        // behaves as a single-user system: it reuses the first Gumi Live user in the
        // database, or creates one if none exists, and returns that ID as game_user_id.
        //
        // Note: this only creates an entry in the gumi_live_users table. The actual
        // game user row in userinfo is created later, after the client sends the
        // CreateUser request.
        std::string currentGumiUser;
        co_await UserInfoService::fetchCurrentGumiUser(theDb(), currentGumiUser);

        // We found a Gumi Live user in the database, so we can just return that one.
        if (!currentGumiUser.empty())
        {
            login.user_id = currentGumiUser;
        }
        else
        {
            // No Gumi Live user exists yet, so we create one with a random ID and return that.
            login.user_id = RandomId();

            // Add him as the only local Gumi Live user.
            co_await gme::nonEmpty(UserInfoService::addGumiUser(
                theDb(),
                login.user_id));

            LOG_DEBUG << "AccountController: new user " << login.user_id;
        }

        login.status = StatusEnum::Success;
        login.token = "test_token"; // Note: Should we use a proper token with drogon session?
        login.status_number = 0;
    }
    catch (const DrogonDbException& ex)
    {
        LOG_ERROR << "AccountController: login failed " << ex.base().what();
    }
    catch (const std::exception& ex)
    {
        LOG_ERROR << "AccountController: login failed " << ex.what();
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

