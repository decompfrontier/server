#include "App.hpp"
#include "AccountController.hpp"
#include <gimuserver/packets/net_gumi_live.hpp>

using namespace drogon;
using namespace drogon::orm;

Task<> AccountController::HandleGuest(HttpRequestPtr rq, std::function<void(const HttpResponsePtr&)> callback)
{
    logReq() << rq;

    // Extract parameters
    const auto& params = rq->getParameters();
    GuestLogin login;
    login.status = v_StatusEnum::Error;
    login.status_number = 1; // TODO: what was this again?

    try
    {
        // TODO: something real should be used here
        login.user_id = "00000001";

        try
        {
            // Check if user exists in users table
            const auto& result = co_await theDb()->execSqlCoro("SELECT id FROM users WHERE id = $1", login.user_id);
            if (result.empty()) {
                // User does not exist, add him to the table
                const auto& cache = theServer()->cache();
                const auto& scfg = cache.serverConfig();
                const auto& def = cache.initializeResp().defines;

                 // everything is hardcoded here, yes!
                co_await theDb()->execSqlCoro("INSERT INTO users(id, account_id, username, admin) VALUES ($1, $2, $3, $4)", login.user_id, "AnyUsername", "GuestUser", 0);
                co_await theDb()->execSqlCoro("INSERT INTO userinfo (id, level, exp, max_unit_count, max_friend_count, zel, karma, brave_coin, max_warehouse_count, want_gift, free_gems, paid_gems, active_deck, summon_tickets, rainbow_coins, colosseum_tickets, active_arena_deck, total_brave_points, avail_brave_points, energy) "
                    "VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10, $11, $12, $13, $14, $15, $16, $17, $18, $19, $20)",
                    // id, level, exp, max_unit_count, max_friend_count
                    login.user_id, scfg.initialLevel, 0, def.max_unit_count, def.max_friend_points,
                    // zel, karma, brave_coin
                    scfg.initialZel, scfg.initialKarma, scfg.initialBraveCoins,
                    // max_warehouse, want_gift, free_gems, paid_gems, active_deck, summon_tickets, rainbow_coins, colosseum_tickets, active_arena_deck, total_brave_points, avail_brave_points, energy
                    def.max_warehouse_count, ""
                    99000000, 99000000, 4200, 5500, "", 10000, 20000, 0, 99, 0, 0, 0, 0, 0, 431);

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
    }
    else
    {
        resp->setStatusCode(k200OK);
        resp->setContentTypeCode(ContentType::CT_APPLICATION_JSON);
        resp->setBody(output);
    }

    callback(resp);
}

