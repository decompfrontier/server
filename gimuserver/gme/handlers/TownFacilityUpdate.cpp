#include "App.hpp"
#include "Handlers.hpp"

#include <gimuserver/gme/common/Common.hpp>

// TownFacilityUpdate (8v43tz7g) — fired when the player confirms a batch of
// facility/location upgrades.  The client sends the COMPLETE desired new state
// for ALL facilities + locations plus the total karma cost of the batch.
// Server persists whatever the client says and deducts karma.
//
// Request struct is generated from KDL (TownFacilityUpdateReq in all.hpp):
//   EuY6L7AX[0].HTVh8a65  — total karma cost
//   YRgx49WG[].y9ET7Aub   — facility_id
//   YRgx49WG[].D9wXQI2V   — lv
//   yj46Q2xw[].un80kW9Y   — location_id
//   yj46Q2xw[].D9wXQI2V   — lv

HANDLEF(TownFacilityUpdate)
{
    LOG_INFO << "TownFacilityUpdate: " << json;

    TownFacilityUpdateReq req{};
    glz::context ctx{};
    if (const auto ec = glz::read<glz::opts{.error_on_unknown_keys = false}>(req, json, ctx); ec)
    {
        LOG_WARN << "TownFacilityUpdate: parse error: " << glz::format_error(ec, json);
        co_return HandleResult::success("{}");
    }

    // Resolve the tutorial-created user; the old hardcoded id matched no rows
    // under the fresh-DB model, so upgrades and karma deduction silently no-op'd.
    const std::string userId    = co_await gme::getSoleUserId(theDb());
    if (userId.empty())
    {
        co_return HandleResult::success("{}");
    }
    const int64_t karmaCost     = req.karma_payment.karma;

    // Persist each facility's new level.  UPSERT: the natural-progression
    // model no longer seeds town rows, so the first update for a facility may
    // arrive before any row exists (row creation normally happens via the
    // town unlock provisioning).
    for (const auto& f : req.facilities)
    {
        try
        {
            co_await theDb()->execSqlCoro(
                "INSERT INTO user_town_facilities (user_id, facility_id, lv) VALUES ($1, $2, $3) "
                "ON CONFLICT(user_id, facility_id) DO UPDATE SET lv=$3;",
                userId, f.facility_id, f.lv);
        }
        catch (const drogon::orm::DrogonDbException& ex)
        {
            LOG_WARN << "TownFacilityUpdate: facility UPSERT failed (id=" << f.facility_id
                     << "): " << ex.base().what();
        }
    }

    // Persist each location's new level.
    for (const auto& l : req.locations)
    {
        try
        {
            co_await theDb()->execSqlCoro(
                "INSERT INTO user_town_locations (user_id, location_id, lv) VALUES ($1, $2, $3) "
                "ON CONFLICT(user_id, location_id) DO UPDATE SET lv=$3;",
                userId, l.location_id, l.lv);
        }
        catch (const drogon::orm::DrogonDbException& ex)
        {
            LOG_WARN << "TownFacilityUpdate: location UPSERT failed (id=" << l.location_id
                     << "): " << ex.base().what();
        }
    }

    // Deduct karma from the user's balance.
    if (karmaCost > 0)
    {
        try
        {
            co_await theDb()->execSqlCoro(
                "UPDATE user_info SET karma=MAX(0, karma-$1) WHERE id=$2;",
                karmaCost, userId);
        }
        catch (const drogon::orm::DrogonDbException& ex)
        {
            LOG_WARN << "TownFacilityUpdate: karma deduct failed: " << ex.base().what();
        }
    }

    co_return HandleResult::success("{}");
}
