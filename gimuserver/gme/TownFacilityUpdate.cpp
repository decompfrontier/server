#include "App.hpp"
#include "Handlers.hpp"

// ---------------------------------------------------------------------------
// Local request types — only used by this handler; no KDL regeneration needed.
// The client sends the COMPLETE new state for all facilities/locations plus the
// total karma cost of the batch.  We just persist whatever the client says.
// ---------------------------------------------------------------------------

struct TownKarmaPayment { int64_t karma = 0; };
template <>
struct glz::meta<TownKarmaPayment> {
    using T = TownKarmaPayment;
    static constexpr auto value = object("HTVh8a65", glz::quoted_num<&T::karma>);
};

struct TownFacilityEntry { int32_t facility_id = 0; int32_t lv = 1; };
template <>
struct glz::meta<TownFacilityEntry> {
    using T = TownFacilityEntry;
    static constexpr auto value = object(
        "y9ET7Aub", glz::quoted_num<&T::facility_id>,
        "D9wXQI2V", glz::quoted_num<&T::lv>
    );
};

struct TownLocationEntry { int32_t location_id = 0; int32_t lv = 1; };
template <>
struct glz::meta<TownLocationEntry> {
    using T = TownLocationEntry;
    static constexpr auto value = object(
        "un80kW9Y", glz::quoted_num<&T::location_id>,
        "D9wXQI2V", glz::quoted_num<&T::lv>
    );
};

struct TownFacilityUpdateReq {
    std::vector<TownKarmaPayment> karma_payment;  // EuY6L7AX[0]
    std::vector<TownFacilityEntry> facilities;     // YRgx49WG[]
    std::vector<TownLocationEntry> locations;      // yj46Q2xw[]
};
template <>
struct glz::meta<TownFacilityUpdateReq> {
    using T = TownFacilityUpdateReq;
    static constexpr auto value = object(
        "EuY6L7AX", &T::karma_payment,
        "YRgx49WG", &T::facilities,
        "yj46Q2xw", &T::locations
    );
};

// ---------------------------------------------------------------------------

HANDLEF(TownFacilityUpdate)
{
    LOG_INFO << "TownFacilityUpdate: " << json;

    TownFacilityUpdateReq req{};
    glz::context ctx{};
    if (const auto ec = glz::read<glz::opts{.error_on_unknown_keys = false}>(req, json, ctx); ec)
    {
        // Parse failure — log but don't close the session; client already
        // updated its own UI state optimistically.
        LOG_WARN << "TownFacilityUpdate: parse error: " << glz::format_error(ec, json);
        co_return HandleResult::success("{}");
    }

    const std::string userId    = "0839899613932562";
    const std::string dbUserId  = "12345678";
    const int64_t karmaCost     = req.karma_payment.empty() ? 0LL : req.karma_payment[0].karma;

    // Persist each facility's new level.
    for (const auto& f : req.facilities)
    {
        try
        {
            co_await theDb()->execSqlCoro(
                "UPDATE user_town_facilities SET lv=$1 WHERE user_id=$2 AND facility_id=$3;",
                f.lv, userId, f.facility_id);
        }
        catch (const drogon::orm::DrogonDbException& ex)
        {
            LOG_WARN << "TownFacilityUpdate: facility UPDATE failed (id=" << f.facility_id
                     << "): " << ex.base().what();
        }
    }

    // Persist each location's new level.
    for (const auto& l : req.locations)
    {
        try
        {
            co_await theDb()->execSqlCoro(
                "UPDATE user_town_locations SET lv=$1 WHERE user_id=$2 AND location_id=$3;",
                l.lv, userId, l.location_id);
        }
        catch (const drogon::orm::DrogonDbException& ex)
        {
            LOG_WARN << "TownFacilityUpdate: location UPDATE failed (id=" << l.location_id
                     << "): " << ex.base().what();
        }
    }

    // Deduct karma from the user's balance.
    if (karmaCost > 0)
    {
        try
        {
            co_await theDb()->execSqlCoro(
                "UPDATE userinfo SET karma=MAX(0, karma-$1) WHERE id=$2;",
                karmaCost, dbUserId);
        }
        catch (const drogon::orm::DrogonDbException& ex)
        {
            LOG_WARN << "TownFacilityUpdate: karma deduct failed: " << ex.base().what();
        }
    }

    co_return HandleResult::success("{}");
}
