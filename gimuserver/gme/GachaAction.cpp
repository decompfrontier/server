#include "App.hpp"
#include "Handlers.hpp"
#include <random>

// GachaAction — perform a summon: pick a unit, insert into user_units, return
// it to the client so the summon animation can play.
//
// Request  (group F7JvPk5H, key bL9fipzaSy7xN2w1):
//   "1IR86sAv": [{
//     "7Ffmi96v": "<gachaId>",          gacha door tapped
//     "a329kbl8": "<#requests>",        usually "1" or "10"
//     "324b023k": <usingGachaTicket>    0 = currency, 1 = summon ticket
//   }]
//
// Response:
//   "qC2tJs4E": [UserUnitInfo]            incremental client-side unit-cache add
//   "Km35HAXv": [OpeUserUnit]             summon-animation array, one per pull
//                                          (results screen renders N tiles)
//   "fEi17cnx": [UserTeamInfo]            refreshed HUD currency after deduction
//
// Currency rules (derived from gacha_info_mst.json + bfdata createbody analysis):
//   - If request `324b023k` == 1 → ticket flow: decrement userinfo.summon_tickets
//     by N (one ticket per pull).  Door's J3stQ7jd/03UGMHxF are ignored.
//   - Else if door's `J3stQ7jd` > 0 → friend-point flow: deduct N * J3stQ7jd
//     from userinfo.friend_point.  This is the Honor Summon family.
//   - Else if door's `03UGMHxF` ∈ (0, 999) → gem flow: deduct N * 03UGMHxF gems,
//     paid_gems first, then free_gems for any remainder.
//   - Else if door's `03UGMHxF` == 999 → ticket-only door (Burst Heroes Selector
//     and similar): user must send ticket flag.  Honor/Gem path rejects with
//     "no currency configured".
//   - If the user can't afford the cost, return HandleResult::error so the
//     dispatcher cleanly fails and the client surfaces an "insufficient
//     currency" banner.
//
// Remaining TODOs:
//   1. Drop-table rolls.  Currently hardcoded to MST id 10017; real BF rolls
//      from gacha_effect_mst.json per-gacha rate buckets keyed by 7Ffmi96v.
//   2. UnitSelectorGachaTicket and GachaFixAction (separate handlers).
//   3. Per-gacha effect_id pick — Km35HAXv.u0vkt9yH currently constant 13762
//      (a Light Featured effect id) regardless of which door rolled.  Wire to
//      the matching GachaEffectMst row when drop-tables land.

// Request shape from bfdata/createbody/GachaActionRequest.txt:
//   addParam("7Ffmi96v", <gachaID string>)     — UserState::getGachaID
//   addParam("a329kbl8", <int>)                 — UserState::noOfGachaRequests
//   addParam("324b023k", <int>)                 — UserState::isUsingGachaTicket
// The C++ `int` overloads on addParam wrap the values as QUOTED strings on the
// wire (BF's createBody helpers stringify everything).  Captured request body:
//   "1IR86sAv":[{"7Ffmi96v":"17160","a329kbl8":"1","324b023k":"0"}]
// All three fields ship as quoted strings — so `using_ticket` MUST be parsed
// with glz::quoted_num (or stored as a string) or the entire body fails to
// deserialize and the dispatcher closes the session with the F7JvPk5H error
// the client renders as a crash.  See handbook §3.1 / §11 quoted-int trap.
struct GachaActionReqItem {
    std::string gacha_id;         // 7Ffmi96v — door id, quoted on wire
    std::string num_requests;     // a329kbl8 — "1" for single, "10" for multi
    int32_t     using_ticket = 0; // 324b023k — 0=currency, 1=summon ticket
};
template <> struct glz::meta<GachaActionReqItem> {
    using T = GachaActionReqItem;
    static constexpr auto value = glz::object(
        "7Ffmi96v", &T::gacha_id,
        "a329kbl8", &T::num_requests,
        "324b023k", glz::quoted_num<&T::using_ticket>
    );
};
struct GachaActionReqBody {
    std::vector<GachaActionReqItem> items;
};
template <> struct glz::meta<GachaActionReqBody> {
    using T = GachaActionReqBody;
    static constexpr auto value = glz::object(
        "1IR86sAv", &T::items
    );
};

// Km35HAXv (OpeUserUnitResponse) — three fields per the readParam audit at
// tools/ida/audits/Km35HAXv_audit.txt (handler at 0x13E8848):
//
//   edy7fq3L → UserState::setSelectUserUnitID(std::string)
//              ALSO append to UserState::GachaUnitsList AND assign to
//              OpeUserUnitInfo's first std::string member.
//   g30VnzQh → UserState::setGachaChangeRateInfo(std::string)
//              ALSO assign to OpeUserUnitInfo's std::string at offset +24.
//              This is the "rate-up info" / rare-up tag for the pulled
//              unit.  Captured wire values we know of are "0" (no rate
//              change) — non-zero would presumably encode a specific rate
//              change effect.  TODO: decode the encoding when a non-zero
//              capture surfaces.
//   u0vkt9yH → GachaEffectMst::setGachaEffectID + UserState::setGachaEffectId.
//              The gacha effect ROW id (joins to GachaEffectMst.u0vkt9yH)
//              that selects which orb-charge / reveal cutscene plays.
//
// All three setters take std::string on the wire (quoted) — we use
// glz::quoted_num to emit numeric-looking values like "0" / "13762" /
// "<user_unit_id>" with the quoted-string shape the client expects.
//
// Wire shape: ARRAY of these objects, one per summoned unit.  See handbook
// §7.8 for the array-vs-object switch and §7.11 for the audit findings.
struct OpeUserUnit {
    int32_t user_unit_id           = 0;     // edy7fq3L — SelectUserUnitID
    int32_t gacha_change_rate_info = 0;     // g30VnzQh — GachaChangeRateInfo (0 = no rate-up)
    int32_t gacha_effect_id        = 13762; // u0vkt9yH — GachaEffectId (joins to GachaEffectMst.id)
};
template <> struct glz::meta<OpeUserUnit> {
    using T = OpeUserUnit;
    static constexpr auto value = glz::object(
        "edy7fq3L", glz::quoted_num<&T::user_unit_id>,
        "g30VnzQh", glz::quoted_num<&T::gacha_change_rate_info>,
        "u0vkt9yH", glz::quoted_num<&T::gacha_effect_id>
    );
};

struct GachaActionRespBody {
    std::vector<UserUnitInfo>           unit_update;
    std::vector<OpeUserUnit>            ope_units;
    UserTeamInfo                        team_info = {};
    std::vector<SummonTicketV2UserInfo> summon_ticket_v2_user;
};
template <> struct glz::meta<GachaActionRespBody> {
    using T = GachaActionRespBody;
    static constexpr auto value = glz::object(
        "qC2tJs4E", &T::unit_update,
        "Km35HAXv", &T::ope_units,
        "fEi17cnx", pkg::glaze::single_array<&T::team_info>(),
        "a3d5d12i", &T::summon_ticket_v2_user
    );
};

static std::string_view gachaAction_elementStr(int32_t e)
{
    switch (e) {
        case 1: return "fire";
        case 2: return "water";
        case 3: return "earth";
        case 4: return "thunder";
        case 5: return "light";
        case 6: return "dark";
        default: return "fire";
    }
}

HANDLEF(GachaAction)
{
    (void)session;
    LOG_INFO << "GachaAction: " << json;

    static constexpr std::string_view kUserId = "0839899613932562";

    // Hardcoded summon unit until the drop-table loader lands.  10017 is the
    // same id the previous build returned and is known to render correctly.
    static constexpr int32_t kSummonMstId = 10017;

    GachaActionReqBody req = {};
    {
        glz::context ctx{};
        if (const auto& ec = glz::read<glz::opts{.error_on_unknown_keys = false}>(req, json, ctx); ec)
        {
            LOG_WARN << "GachaAction: bad request JSON: " << glz::format_error(ec, json);
            co_return HandleResult::error("Deserialization error");
        }
    }
    const std::string gachaIdStr = req.items.empty() ? std::string{} : req.items[0].gacha_id;
    const std::string numReqStr  = req.items.empty() ? std::string{} : req.items[0].num_requests;
    LOG_INFO << "GachaAction: gacha_id=" << gachaIdStr
             << " requests=" << numReqStr
             << " ticket="   << (req.items.empty() ? 0 : req.items[0].using_ticket);

    // Parse the pull-count.  Multi-summon sends "10"; single sends "1".  Clamp
    // to a sane range to defend against malformed inputs.  Default 1.
    int32_t pulls = 1;
    if (!numReqStr.empty()) {
        try { pulls = std::stoi(numReqStr); } catch (...) {}
    }
    if (pulls < 1)   pulls = 1;
    if (pulls > 11)  pulls = 11;  // BF caps multi-pull at 11 (10+1 bonus)

    const int32_t usingTicket = req.items.empty() ? 0 : req.items[0].using_ticket;

    // ----------------------------------------------------------------------
    // Look up the gacha door in the catalog so we can read its cost fields.
    // gacha_info_mst.json is loaded into m_userrsp.gacha_info at boot; entries are
    // keyed by `id` (7Ffmi96v on wire, generated as `int32_t GachaInfoMst::id`).
    // ----------------------------------------------------------------------
    int32_t gachaIdNum = 0;
    if (!gachaIdStr.empty()) {
        try { gachaIdNum = std::stoi(gachaIdStr); } catch (...) {}
    }
    const auto& catalog = theServer()->cache().userInfoResp().gacha_info;
    const GachaInfoMst* door = nullptr;
    for (const auto& e : catalog) { if (e.id == gachaIdNum) { door = &e; break; } }
    if (!door)
    {
        LOG_WARN << "GachaAction: gacha id " << gachaIdNum << " not in gacha_info catalog";
        co_return HandleResult::error("GachaAction: unknown gacha door");
    }

    // ----------------------------------------------------------------------
    // Determine the cost.  See file-header comment for the rule table.  We
    // compute the deduction up front so we can reject pulls the player can't
    // afford BEFORE we INSERT any units — avoids the "I spent nothing but got
    // 10 units" exploit if the response build later fails.
    // ----------------------------------------------------------------------
    enum class CostKind { None, FriendPoint, Gem, SummonTicket };
    CostKind costKind = CostKind::None;
    int64_t  totalCost = 0;

    const int32_t fpCost  = door->need_friends_point;   // J3stQ7jd
    const int32_t gemCost = door->brave_coin;           // 03UGMHxF — gem cost per pull

    if (usingTicket == 1) {
        costKind  = CostKind::SummonTicket;
        totalCost = pulls;                              // 1 ticket per pull
    } else if (fpCost > 0) {
        costKind  = CostKind::FriendPoint;
        totalCost = static_cast<int64_t>(fpCost) * pulls;
    } else if (gemCost > 0 && gemCost != 999) {
        costKind  = CostKind::Gem;
        totalCost = static_cast<int64_t>(gemCost) * pulls;
    } else {
        // gemCost == 999 sentinel (Burst Heroes Selector etc.) or both fields
        // zero — door isn't payable via currency, only via ticket.
        LOG_WARN << "GachaAction: door " << gachaIdNum
                 << " has no currency cost (fp=" << fpCost << " gem=" << gemCost
                 << ") and request didn't set using_ticket=1";
        co_return HandleResult::error("GachaAction: this door requires a ticket");
    }

    // ----------------------------------------------------------------------
    // Verify the user can afford the cost.  Single SELECT to read all the
    // relevant currency columns.
    // ----------------------------------------------------------------------
    const std::string userIdStr = std::string(kUserId);
    const auto balRows = co_await theDb()->execSqlCoro(
        "SELECT friend_point, paid_gems, free_gems, summon_tickets"
        " FROM userinfo WHERE id=$1;",
        userIdStr);
    if (balRows.empty())
    {
        LOG_ERROR << "GachaAction: userinfo row missing";
        co_return HandleResult::error("GachaAction: userinfo missing");
    }
    const auto& bal = balRows[0];
    const int32_t haveFp     = bal["friend_point"].as<int32_t>();
    const int32_t havePaid   = bal["paid_gems"].as<int32_t>();
    const int32_t haveFree   = bal["free_gems"].as<int32_t>();
    const int32_t haveTicket = bal["summon_tickets"].as<int32_t>();

    // For the ticket branch: look up which V2 ticket type (if any) is bound
    // to this door, then check the user's owned count in user_summon_tickets_v2.
    // `target_gacha` on SummonTicketV2Mst is a string ("17160" or "" for
    // universal) so we compare as string.  When no matching type exists, the
    // ticket flow falls back to the generic `summon_tickets` count only.
    int32_t v2TicketId       = 0;       // 0 = "no V2 mapping for this door"
    int32_t v2OwnedForDoor   = 0;
    if (costKind == CostKind::SummonTicket)
    {
        const auto& v2Cat = theServer()->cache().userInfoResp().summon_ticket_v2;
        for (const auto& t : v2Cat)
        {
            // Both fields are std::optional<...> in the generated SummonTicketV2Mst
            // (they're marked `optional #true` in the KDL because some rows leave
            // them empty for universal tickets).  Skip rows without target_gacha,
            // then unwrap before compare.
            if (!t.target_gacha.has_value() || t.target_gacha->empty()) continue;
            if (!t.id.has_value()) continue;
            if (*t.target_gacha == gachaIdStr)
            {
                v2TicketId = *t.id;
                break;
            }
        }
        if (v2TicketId > 0)
        {
            const auto v2Rows = co_await theDb()->execSqlCoro(
                "SELECT count FROM user_summon_tickets_v2"
                " WHERE user_id=$1 AND ticket_id=$2;",
                userIdStr, v2TicketId);
            if (!v2Rows.empty())
                v2OwnedForDoor = v2Rows[0]["count"].as<int32_t>();
        }
    }

    auto reject = [&](const char* reason) -> HandleResult {
        LOG_WARN << "GachaAction: " << reason
                 << " (need " << totalCost << ", have fp=" << haveFp
                 << " paid=" << havePaid << " free=" << haveFree
                 << " ticket=" << haveTicket
                 << " v2[" << v2TicketId << "]=" << v2OwnedForDoor << ")";
        return HandleResult::error(reason);
    };

    switch (costKind) {
        case CostKind::FriendPoint:
            if (haveFp < totalCost) co_return reject("Insufficient honor points");
            break;
        case CostKind::Gem:
            if (static_cast<int64_t>(havePaid) + haveFree < totalCost)
                co_return reject("Insufficient gems");
            break;
        case CostKind::SummonTicket:
            // Need BOTH: enough generic tickets (the HUD counter) AND enough
            // of the per-type V2 inventory (the gacha screen's per-door
            // ticket counter).  When the door has no V2 binding, v2TicketId=0
            // and the V2 check trivially passes.
            if (haveTicket < totalCost) co_return reject("Insufficient summon tickets");
            if (v2TicketId > 0 && v2OwnedForDoor < totalCost)
                co_return reject("Insufficient tickets of this type");
            break;
        case CostKind::None:
            break;  // unreachable
    }

    // Resolve the target MST so we can populate the new row's base stats.
    const auto& unitMst = theServer()->cache().unitMst();
    const UnitMst* mst = nullptr;
    for (const auto& u : unitMst) { if (u.id == kSummonMstId) { mst = &u; break; } }
    if (!mst)
    {
        LOG_ERROR << "GachaAction: MST id " << kSummonMstId << " not found in unitMst cache";
        co_return HandleResult::error("GachaAction: unknown unit");
    }

    const int32_t skillLv      = mst->skill_id       > 0 ? 10 : 0;
    const int32_t extraSkillLv = mst->extra_skill_id > 0 ? 10 : 0;

    // Roll a unit_type_id per pull (Lord/Anima/Breaker/Guardian/Oracle/Rex) —
    // same per-unit randomisation that GimuServer::InsertUnitFromMst uses for
    // the seed pass.
    static thread_local std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int32_t> typeDist(1, 6);

    GachaActionRespBody resp = {};
    int32_t firstUserUnitId = 0;

    std::vector<int32_t> unitTypeIds;
    unitTypeIds.reserve(pulls);
    for (int32_t i = 0; i < pulls; ++i) unitTypeIds.push_back(typeDist(rng));

    const std::string& userId = userIdStr;   // already constructed above for the balance check
    const std::string unitId  = std::to_string(kSummonMstId);
    const std::string element = std::string(gachaAction_elementStr(mst->element));

    // Build ONE batched INSERT with all `pulls` rows.  Strings ($1, $2, $3) are
    // SQL-bound; integer values (mst stats, skill ids, unit_type_ids) come from
    // trusted server sources (UnitMst + our RNG) and are inlined directly into
    // the VALUES clause — no injection surface, and we avoid Drogon's
    // variadic-template arity limits for the parameter pack.
    //
    // History: an earlier version awaited 20 separate execSqlCoro calls in a
    // 10-iteration loop (INSERT + SELECT last_insert_rowid per pull).  With
    // Drogon's `connection_numbr: 1` SQLite pool (deploy/config.json) that
    // hung the entire server process at N=10 — the single worker thread
    // never got a chance to drain the queue.  Batching to one INSERT + one
    // SELECT works.  See handbook §6.14.
    std::string sqlInlined =
        "INSERT INTO user_units "
        "(user_id, unit_id, unit_lv,"
        " base_hp,  add_hp,  ext_hp,  limit_over_hp,"
        " base_atk, add_atk, ext_atk, limit_over_atk,"
        " base_def, add_def, ext_def, limit_over_def,"
        " base_heal,add_heal,ext_heal,limit_over_heal,"
        " exp, total_exp,"
        " skill_id, skill_lv, extra_skill_id, extra_skill_lv, leader_skill_id,"
        " element, fe_bp, fe_max_usable_bp, unit_type_id) VALUES ";

    for (int32_t pull = 0; pull < pulls; ++pull)
    {
        if (pull) sqlInlined += ',';
        sqlInlined += "($1,$2,1,";
        sqlInlined += std::to_string(mst->min_hp);   sqlInlined += ",0,0,0,";
        sqlInlined += std::to_string(mst->min_atk);  sqlInlined += ",0,0,0,";
        sqlInlined += std::to_string(mst->min_def);  sqlInlined += ",0,0,0,";
        sqlInlined += std::to_string(mst->min_rec);  sqlInlined += ",0,0,0,1,1,";
        sqlInlined += std::to_string(mst->skill_id);       sqlInlined += ',';
        sqlInlined += std::to_string(skillLv);             sqlInlined += ',';
        sqlInlined += std::to_string(mst->extra_skill_id); sqlInlined += ',';
        sqlInlined += std::to_string(extraSkillLv);        sqlInlined += ',';
        sqlInlined += std::to_string(mst->leader_skill_id);sqlInlined += ",$3,100,200,";
        sqlInlined += std::to_string(unitTypeIds[pull]);
        sqlInlined += ')';
    }
    sqlInlined += ';';

    LOG_INFO << "GachaAction: batched INSERT of " << pulls << " rows";
    co_await theDb()->execSqlCoro(sqlInlined, userId, unitId, element);

    // Step 2 (single SELECT): get last_insert_rowid().  SQLite increments
    // monotonically within a single statement, so the N inserted rows have
    // ids `last - N + 1 .. last`.
    const auto idRows = co_await theDb()->execSqlCoro("SELECT last_insert_rowid() AS id;");
    if (idRows.empty())
    {
        LOG_ERROR << "GachaAction: last_insert_rowid returned no row";
        co_return HandleResult::error("GachaAction: insert id lookup failed");
    }
    const int32_t lastUserUnitId = idRows[0]["id"].as<int32_t>();
    firstUserUnitId = lastUserUnitId - pulls + 1;

    for (int32_t pull = 0; pull < pulls; ++pull)
    {
        UserUnitInfo d = {};
        d.user_id            = userId;
        d.user_unit_id       = firstUserUnitId + pull;
        d.unit_id            = kSummonMstId;
        d.unit_type_id       = unitTypeIds[pull];
        d.unit_lv            = 1;
        d.exp                = 1;
        d.total_exp          = 1;
        d.base_hp            = mst->min_hp;
        d.base_atk           = mst->min_atk;
        d.base_def           = mst->min_def;
        d.base_heal          = mst->min_rec;
        d.element            = element;
        d.leader_skill_id    = mst->leader_skill_id;
        d.skill_id           = mst->skill_id;
        d.skill_lv           = skillLv;
        d.extra_skill_id     = mst->extra_skill_id;
        d.extra_skill_lv     = extraSkillLv;
        d.fe_bp              = 100;
        d.fe_max_usable_bp   = 200;
        d.new_flag           = true;
        resp.unit_update.emplace_back(std::move(d));
    }

    LOG_INFO << "GachaAction: inserted " << pulls << " unit(s); ids "
             << firstUserUnitId << ".." << lastUserUnitId;

    // Emit one Km35HAXv entry per pull so the "Summon Results" screen
    // renders N reveal tiles (not just one).  Each entry points to the
    // DB row id of its corresponding pulled unit; gate_anim_flag stays at
    // the legacy 13762 constant for now (proper per-rarity effect-id pick
    // is the GachaEffectMst follow-up — see handbook §7.8).
    resp.ope_units.reserve(pulls);
    for (int32_t pull = 0; pull < pulls; ++pull)
    {
        OpeUserUnit o = {};
        o.user_unit_id           = firstUserUnitId + pull;
        o.gacha_change_rate_info = 0;       // no rate-up (per audit semantic)
        o.gacha_effect_id        = 13762;   // hardcoded — see §7.8 follow-up TODO
        resp.ope_units.emplace_back(o);
    }

    // ----------------------------------------------------------------------
    // Deduct the cost.  One UPDATE per currency branch — kept as a single
    // statement so the SQLite worker thread stays unwedged (see §6.14).
    // For gems we drain paid_gems first then free_gems, matching BF's
    // documented spending order.
    // ----------------------------------------------------------------------
    switch (costKind) {
        case CostKind::FriendPoint:
            co_await theDb()->execSqlCoro(
                "UPDATE userinfo SET friend_point = MAX(0, friend_point - $1)"
                " WHERE id=$2;",
                static_cast<int32_t>(totalCost), userId);
            break;
        case CostKind::Gem: {
            const int32_t fromPaid = std::min(havePaid, static_cast<int32_t>(totalCost));
            const int32_t fromFree = static_cast<int32_t>(totalCost) - fromPaid;
            co_await theDb()->execSqlCoro(
                "UPDATE userinfo"
                " SET paid_gems = paid_gems - $1,"
                "     free_gems = free_gems - $2"
                " WHERE id=$3;",
                fromPaid, fromFree, userId);
            break;
        }
        case CostKind::SummonTicket:
            // Drain the generic counter (`9r3aLmaB` in team_info).
            co_await theDb()->execSqlCoro(
                "UPDATE userinfo SET summon_tickets = MAX(0, summon_tickets - $1)"
                " WHERE id=$2;",
                static_cast<int32_t>(totalCost), userId);
            // ALSO drain the per-type V2 row so the gacha screen's per-door
            // ticket count (a3d5d12i entries with matching id) drops.  Skipped
            // when the door has no V2 binding (door 1001 / Honor for example).
            if (v2TicketId > 0)
            {
                co_await theDb()->execSqlCoro(
                    "UPDATE user_summon_tickets_v2"
                    " SET count = MAX(0, count - $1)"
                    " WHERE user_id=$2 AND ticket_id=$3;",
                    static_cast<int32_t>(totalCost), userId, v2TicketId);
            }
            break;
        case CostKind::None:
            break;
    }

    // ----------------------------------------------------------------------
    // Fetch refreshed userinfo + populate team_info so the HUD reflects the
    // deduction immediately (otherwise the gem/HP counters stay at the
    // pre-summon value until the next UserInfo / GachaList refresh).
    // ----------------------------------------------------------------------
    const auto infoRows = co_await theDb()->execSqlCoro(
        "SELECT level, exp, zel, karma, brave_coin, free_gems, paid_gems, energy,"
        " max_unit_count, max_warehouse_count, summon_tickets, rainbow_coins,"
        " colosseum_tickets, friend_point, total_brave_points, avail_brave_points,"
        " active_deck, want_gift FROM userinfo WHERE id=$1;",
        userId);
    if (!infoRows.empty())
    {
        const auto& row   = infoRows[0];
        const int32_t lvl = row["level"].as<int32_t>();
        const auto& prog  = theServer()->cache().initializeResp().progression;
        const UserLevelMst* lv = nullptr;
        for (const auto& e : prog) { if (e.level == lvl) { lv = &e; break; } }

        auto& ti = resp.team_info;
        ti.user_id              = userId;
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
    }

    // ----------------------------------------------------------------------
    // Refresh the V2 typed-ticket inventory in the response so the gacha
    // screen's per-door ticket button reflects the new count immediately.
    // Mirrors the seed logic in UserInfo.cpp: one entry per ticket TYPE
    // carrying both id (b0D2iq2d) and amount (Rs7bCE3t).  See the readParam
    // audit at tools/ida/audits/a3d5d12i_audit.txt and handbook §7.11.
    // Emitting even when amount hasn't changed (non-ticket pulls) keeps the
    // client cache aligned with no per-branch logic.
    // ----------------------------------------------------------------------
    {
        const auto ticketRows = co_await theDb()->execSqlCoro(
            "SELECT ticket_id, count FROM user_summon_tickets_v2"
            " WHERE user_id=$1 AND count > 0;",
            userId);
        for (const auto& trow : ticketRows)
        {
            SummonTicketV2UserInfo t = {};
            t.id     = trow["ticket_id"].as<int32_t>();
            t.amount = trow["count"].as<int32_t>();
            resp.summon_ticket_v2_user.emplace_back(t);
        }
    }

    std::string buffer{};
    if (const auto& ec = glz::write_json(resp, buffer); ec)
    {
        LOG_ERROR << "GachaAction: serialization error: " << glz::format_error(ec, buffer);
        co_return HandleResult::error("Serialization error");
    }

    co_return HandleResult::success(buffer);
}
