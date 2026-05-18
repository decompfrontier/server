#include "App.hpp"
#include "Handlers.hpp"

// GachaList — return the catalog of summon doors for the gacha menu.
//
// Request  (group Uo86DcRh, key 8JbxFvuSaB2CK7Ln):
//   No fields specific to this handler — just the createUserInfoTag +
//   signal-key + version envelope (see bfdata/createbody/GachaListRequest.txt).
//
// Response:
//   "6FrKacq7": [{"Kn51uR4Y":"axhp8Sin"}]   SignalKey dispatch tag
//   "fEi17cnx": [UserTeamInfo]              refreshed currency / counts
//   "1IR86sAv": [GachaInfoMst...]           door catalog from deploy/system/gacha_info_mst.json
//   "IBs49NiH": [GachaCategory...]          banner categories (Veteran / Honor / etc.)
//                                            from deploy/system/gacha_category_mst.json
//
// Without the IBs49NiH categories the summon screen renders no banner tiles —
// each category is one horizontal banner whose `3rCmq58M` (gatcha_id_list)
// references door ids in 1IR86sAv.  The client filters by start/end date, so
// only categories with current-date validity appear on screen.

struct GachaListRespBody {
    SignalKey                  signal_key = {};
    UserTeamInfo               team_info  = {};
    std::vector<GachaInfoMst>  gacha_info;
    std::vector<GachaCategory> gacha_category;
};
template <> struct glz::meta<GachaListRespBody> {
    using T = GachaListRespBody;
    static constexpr auto value = glz::object(
        "6FrKacq7", pkg::glaze::single_array<&T::signal_key>(),
        "fEi17cnx", pkg::glaze::single_array<&T::team_info>(),
        "1IR86sAv", &T::gacha_info,
        "IBs49NiH", &T::gacha_category
    );
};

HANDLEF(GatchaList)
{
    (void)session;
    LOG_INFO << "GatchaList: " << json;

    static constexpr std::string_view kUserId = "0839899613932562";

    const auto infoRows = co_await theDb()->execSqlCoro(
        "SELECT level, exp, zel, karma, brave_coin, free_gems, paid_gems, energy,"
        " max_unit_count, max_warehouse_count, summon_tickets, rainbow_coins,"
        " colosseum_tickets, friend_point, total_brave_points, avail_brave_points,"
        " active_deck, want_gift FROM userinfo WHERE id=$1;",
        std::string(kUserId)
    );
    if (infoRows.empty())
    {
        LOG_ERROR << "GatchaList: userinfo row missing";
        co_return HandleResult::error("GatchaList: userinfo missing");
    }
    const auto& row   = infoRows.at(0);
    const int32_t lvl = row["level"].as<int32_t>();

    const auto& prog = theServer()->cache().initializeResp().progression;
    const UserLevelMst* lv = nullptr;
    for (const auto& e : prog) { if (e.level == lvl) { lv = &e; break; } }

    GachaListRespBody resp = {};
    resp.signal_key.key = "axhp8Sin";

    auto& ti = resp.team_info;
    ti.user_id              = std::string(kUserId);
    ti.level                = lvl;
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

    resp.gacha_info     = theServer()->cache().userInfoResp().gacha_info;
    resp.gacha_category = theServer()->cache().gachaCategories();

    std::string buffer{};
    if (const auto& ec = glz::write_json(resp, buffer); ec)
    {
        LOG_ERROR << "GatchaList: serialization error: " << glz::format_error(ec, buffer);
        co_return HandleResult::error("Serialization error");
    }

    co_return HandleResult::success(buffer);
}
