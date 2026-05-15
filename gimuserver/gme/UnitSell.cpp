#include "App.hpp"
#include "Handlers.hpp"

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

static UserTeamInfo unitSell_buildTeamInfo(const drogon::orm::Row& row,
                                            const std::vector<UserLevelMst>& prog)
{
    const int32_t level = row["level"].as<int32_t>();
    const UserLevelMst* lv = nullptr;
    for (const auto& e : prog) { if (e.level == level) { lv = &e; break; } }

    UserTeamInfo ti = {};
    ti.user_id              = "0839899613932562";
    ti.level                = level;
    ti.exp                  = row["exp"].as<int64_t>();
    ti.zel                  = row["zel"].as<int64_t>();
    ti.karma                = row["karma"].as<int64_t>();
    ti.brave_coin           = row["brave_coin"].as<int32_t>();
    ti.action_point         = row["energy"].as<int32_t>();
    ti.max_action_point     = lv ? lv->action_points    : 100;
    ti.deck_cost            = lv ? lv->deck_cost        : 20;
    ti.max_friend_count     = lv ? lv->friend_count     : 50;
    ti.add_friend_count     = lv ? lv->add_friend_count : 0;
    ti.max_unit_count       = row["max_unit_count"].as<int32_t>();
    ti.warehouse_count      = row["max_warehouse_count"].as<int32_t>();
    ti.active_deck          = row["active_deck"].as<int32_t>();
    ti.summon_ticket        = row["summon_tickets"].as<int32_t>();
    ti.rainbow_coin         = row["rainbow_coins"].as<int32_t>();
    ti.colosseum_ticket     = row["colosseum_tickets"].as<int32_t>();
    ti.friend_point         = row["friend_point"].as<int32_t>();
    ti.brave_points_total   = row["total_brave_points"].as<int32_t>();
    ti.current_brave_points = row["avail_brave_points"].as<int32_t>();
    ti.want_gift            = row["want_gift"].as<std::string>();
    ti.paid_gems            = row["paid_gems"].as<int32_t>();
    ti.free_gems            = row["free_gems"].as<int32_t>();
    ti.reinforcement_deck.emplace_back(0);
    ti.reinforcement_deck.emplace_back(0);
    ti.reinforcement_deck.emplace_back(0);
    return ti;
}

// ---------------------------------------------------------------------------
// Handler
// ---------------------------------------------------------------------------
HANDLEF(UnitSell)
{
    (void)session;
    LOG_INFO << "UnitSell: " << json;

    static constexpr std::string_view kUserId   = "0839899613932562";

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
        "SELECT unit_id FROM user_units WHERE user_id=$1 AND id IN (" + idList + ");",
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

    // Step 2: delete sold units.
    co_await theDb()->execSqlCoro(
        "DELETE FROM user_units WHERE user_id=$1 AND id IN (" + idList + ");",
        std::string(kUserId)
    );

    // Step 3: credit zel.
    co_await theDb()->execSqlCoro(
        "UPDATE userinfo SET zel = zel + $1 WHERE id=$2;",
        totalZel, std::string(kUserId)
    );

    // Step 4: fetch fresh userinfo to build accurate team_info.
    const auto infoRows = co_await theDb()->execSqlCoro(
        "SELECT level, exp, zel, karma, brave_coin, free_gems, paid_gems, energy,"
        " max_unit_count, max_warehouse_count, summon_tickets, rainbow_coins,"
        " colosseum_tickets, friend_point, total_brave_points, avail_brave_points,"
        " active_deck, want_gift FROM userinfo WHERE id=$1;",
        std::string(kUserId)
    );

    UnitSellRespBody resp = {};
    resp.team_info = unitSell_buildTeamInfo(
        infoRows.at(0),
        theServer()->cache().initializeResp().progression
    );

    std::string buffer{};
    if (const auto& ec2 = glz::write_json(resp, buffer); ec2)
    {
        LOG_ERROR << "UnitSell: serialization error: " << glz::format_error(ec2, buffer);
        co_return HandleResult::error("Serialization error");
    }

    co_return HandleResult::success(buffer);
}
