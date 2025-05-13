#include "App.hpp"
#include "AccountController.hpp"
#include <gimuserver/packets/net_gumi_live.hpp>

using namespace drogon;
using namespace drogon::orm;

Task<> AccountController::HandleGuest(HttpRequestPtr rq, std::function<void(const HttpResponsePtr&)>&& callback)
{
    LOG_REQ << rq;

    // Extract parameters
    auto params = rq->getParameters();
    GuestLogin login;
    login.status = v_StatusEnum::Error;
    login.status_number = 1; // TODO: what was this again?

    try
    {
        std::string strUid = params["ak"].empty() ? "00000000" : params["ak"]; // Use 'ak' as userId (e.g., 0101AABB or 0839899613932562) default_user_id
        login.user_id = static_cast<uint32_t>(std::stoul(strUid, nullptr, 16));

        std::string deviceId = params["device_id"].empty() ? "unknown_device" : params["device_id"];

        try
        {
            // Check if user exists in users table
            const auto& result = co_await GME_DB->execSqlCoro("SELECT id FROM users WHERE id = $1", login.user_id);
            if (result.empty()) {
                // User does not exist, add him to the table
                co_await GME_DB->execSqlCoro("INSERT INTO users(id, account_id, username, admin) VALUES ($1, $2, $3, $4)", login.user_id, deviceId, "GuestUser", 0);
                LOG_DEBUG << "AccountController: new user " << login.user_id << " (device: " << deviceId << ")";
            }

            login.status = v_StatusEnum::Success;
            login.token = "TEST_SESSION_TOKEN"; // TODO: use a proper token with drogon session also
            login.status_number = 0;

        }
        catch (drogon::orm::DrogonDbException ex)
        {
            LOG_ERROR << "AccountController: Query execution failed: " << ex.base().what();
        }
    }
    catch (std::invalid_argument ex)
    {
        LOG_DEBUG << "AccountController: invalid ak parameter " << params["ak"];
    }

    auto resp = HttpResponse::newHttpResponse();

    std::string output{};
    auto ec = glz::write_json(login, output);
    if (ec)
    {
        LOG_ERROR << "AccountController: Cannot serialize GuestLogin " << ec;
        resp->setStatusCode(k500InternalServerError);
    }
    else
    {
        resp->setContentTypeCode(ContentType::CT_APPLICATION_JSON);
        resp->setBody(output);
    }

    callback(resp);

#if 0 // TODO: this should not be set in the gumi account controller
    GME_DB->execSqlAsync(
        "SELECT id FROM users WHERE id = $1",
        [this, userId, deviceId, callback](const drogon::orm::Result& result) {

            GME_DB->execSqlAsync(
                "INSERT INTO userinfo (id, level, exp, max_unit_count, max_friend_count, zel, karma, brave_coin, max_warehouse_count, want_gift, free_gems, paid_gems, active_deck, summon_tickets, rainbow_coins, colosseum_tickets, active_arena_deck, total_brave_points, avail_brave_points, energy) "
                "VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10, $11, $12, $13, $14, $15, $16, $17, $18, $19, $20)",
                [](const drogon::orm::Result& result) {
                LOG_INFO << "AccountController: Inserted default userinfo for new user";
            },
                [](const drogon::orm::DrogonDbException& e) {
                LOG_ERROR << "AccountController: Failed to insert default userinfo: " << e.base().what();
            },
                userId, 999, 0, 4000, 50, 99000000, 99000000, 4200, 5500, "", 10000, 20000, 0, 99, 0, 0, 0, 0, 0, 431
            ); // Sets new user values if no gme.sqlite exists :)
        }
#endif
}

