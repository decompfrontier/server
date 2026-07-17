#include "App.hpp"
#include "Handlers.hpp"

#include <gimuserver/gme/common/Common.hpp>

// UnitSell — sell one or more owned units for zel.
//
// Request  (group Ri3uTq9b, key 92VqcGFWuPkmT60U):
//   "Km35HAXv": [ {"edy7fq3L": "<userUnitId>"}, ... ]
//
// Response:
//   "fEi17cnx": [UserTeamInfo]  — refreshes zel counter in the client HUD.
//
// Zel formula: sum UnitMst.sell_price across sold units (server-authoritative).

// ---------------------------------------------------------------------------
// Request parsing structs (file-scope so glz::meta<> specialisations compile)
// ---------------------------------------------------------------------------
struct UnitSellEntry {
    int32_t user_unit_id = 0;
};
template <> struct glz::meta<UnitSellEntry> {
    using T = UnitSellEntry;
    static constexpr auto value = glz::object(
        "edy7fq3L", glz::quoted_num<&T::user_unit_id>
    );
};
struct UnitSellReqBody {
    std::vector<UnitSellEntry> units;
};
template <> struct glz::meta<UnitSellReqBody> {
    using T = UnitSellReqBody;
    static constexpr auto value = glz::object(
        "Km35HAXv", &T::units
    );
};

// ---------------------------------------------------------------------------
// Response wrapper (only fEi17cnx needed for sell)
// ---------------------------------------------------------------------------
struct UnitSellRespBody {
    UserTeamInfo team_info = {};
};
template <> struct glz::meta<UnitSellRespBody> {
    using T = UnitSellRespBody;
    static constexpr auto value = glz::object(
        "fEi17cnx", pkg::glaze::single_array<&T::team_info>()
    );
};

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------
static std::string unitSell_stripSuffix(const std::string& raw)
{
    auto pos = raw.find('_');
    return (pos != std::string::npos) ? raw.substr(0, pos) : raw;
}

// ---------------------------------------------------------------------------
// Handler
// ---------------------------------------------------------------------------
HANDLEF(UnitSell)
{
    (void)session;
    LOG_INFO << "UnitSell: " << json;

    // Transitional bridge: resolve the sole offline user at runtime
    // (tutorial-created).  TODO port to gme::getUserIdentity.
    const std::string kUserId = co_await gme::getSoleUserId(theDb());

    // Parse request.  Allow unknown keys so any extra client fields don't abort parsing.
    UnitSellReqBody req = {};
    {
        glz::context ctx{};
        if (const auto& ec = glz::read<glz::opts{.error_on_unknown_keys = false}>(req, json, ctx); ec)
        {
            LOG_WARN << "UnitSell: bad request JSON: " << glz::format_error(ec, json);
            co_return HandleResult::error("Deserialization error");
        }
    }
    if (req.units.empty())
    {
        LOG_WARN << "UnitSell: empty unit list";
        co_return HandleResult::error("UnitSell: empty unit list");
    }

    // Build a SQL IN-clause from validated ids.
    std::string idList;
    for (size_t i = 0; i < req.units.size(); ++i)
    {
        if (i) idList += ',';
        idList += std::to_string(req.units[i].user_unit_id);
    }

    // Step 1: look up sell price for each unit via the MST cache.
    const auto& unitMst = theServer()->cache().unitMst();
    const auto unitRows = co_await theDb()->execSqlCoro(
        "SELECT unit_id FROM user_units WHERE user_id=$1 AND user_unit_id IN (" + idList + ");",
        std::string(kUserId)
    );

    int64_t totalZel = 0;
    for (const auto& row : unitRows)
    {
        const std::string mstId = unitSell_stripSuffix(row["unit_id"].as<std::string>());
        const int32_t mstIdInt  = std::stoi(mstId);
        auto it = std::find_if(unitMst.begin(), unitMst.end(),
            [mstIdInt](const UnitMst& u) { return u.id == mstIdInt; });
        if (it != unitMst.end())
            totalZel += it->sell_price;
    }

    LOG_INFO << "UnitSell: selling " << unitRows.size() << " units, total zel gain=" << totalZel;

    // Step 2: return any spheres equipped on the sold units to the warehouse,
    // then delete the units.  Without the return, the equipped items would be
    // destroyed with the row.
    co_await gme::returnEquippedSpheres(
        theDb(), gme::UserIdentity{ .userId = kUserId }, idList);
    co_await theDb()->execSqlCoro(
        "DELETE FROM user_units WHERE user_id=$1 AND user_unit_id IN (" + idList + ");",
        std::string(kUserId)
    );

    // Step 3: credit zel.
    co_await theDb()->execSqlCoro(
        "UPDATE user_info SET zel = zel + $1 WHERE id=$2;",
        totalZel, std::string(kUserId)
    );

    UnitSellRespBody resp = {};
    resp.team_info = std::move(
        (co_await gme::getTeamInfo(theDb(),
            gme::UserIdentity{.userId = std::string(kUserId)})).nonEmpty());

    std::string buffer{};
    if (const auto& ec2 = glz::write_json(resp, buffer); ec2)
    {
        LOG_ERROR << "UnitSell: serialization error: " << glz::format_error(ec2, buffer);
        co_return HandleResult::error("Serialization error");
    }

    co_return HandleResult::success(buffer);
}
