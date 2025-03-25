#include "AccountController.hpp"
#include "core/Utils.hpp"
#include <db/DbMacro.hpp>

void AccountController::HandleGuest(const HttpRequestPtr& rq, std::function<void(const HttpResponsePtr&)>&& callback)
{
    Utils::DumpInfoToDrogon(rq, "AccountController");

    // Extract parameters
    auto params = rq->getParameters();
    std::string userId = params["ak"].empty() ? "Developer" : params["ak"]; // Use 'ak' as userId (e.g., 0101AABB or 0839899613932562) default_user_id
    std::string deviceId = params["device_id"].empty() ? "unknown_device" : params["device_id"];

    // Check if user exists in users table
    GME_DB->execSqlAsync(
        "SELECT id FROM users WHERE id = $1",
        [this, userId, deviceId, callback](const drogon::orm::Result& result) {
        if (result.empty()) {
            // New user: Insert into users and userinfo
            GME_DB->execSqlAsync(
                "INSERT INTO users (id, account_id, username, admin) VALUES ($1, $2, $3, $4)",
                [](const drogon::orm::Result& result) {
                LOG_INFO << "AccountController: Inserted new user";
            },
                [](const drogon::orm::DrogonDbException& e) {
                LOG_ERROR << "AccountController: Failed to insert new user: " << e.base().what();
            },
                userId, deviceId, "GuestUser", 0
            );

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

        // Return login response
        Json::Value v;
        v["status"] = "successful";
        v["token"] = "test_token"; // TODO: Generate a proper token
        v["game_user_id"] = userId; // Use the same userId as in the database
        v["status_no"] = "0";
        callback(HttpResponse::newHttpJsonResponse(v));
    },
        [callback](const drogon::orm::DrogonDbException& e) {
        LOG_ERROR << "AccountController: Database error during login: " << e.base().what();
        auto resp = HttpResponse::newHttpResponse();
        resp->setStatusCode(k500InternalServerError);
        callback(resp);
    },
        userId
    );
}