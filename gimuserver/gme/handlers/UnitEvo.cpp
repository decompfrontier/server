#include "App.hpp"
#include "Handlers.hpp"

#include <gimuserver/gme/common/Common.hpp>

// UnitEvo — evolve a unit into its next form.
//
// Request  (group 0gUSE84e, key biHf01DxcrPou5Qt):
//   "Km35HAXv": [{"edy7fq3L":"<baseId>","mnZ5K4Ii":"1"},   // base unit  (role 1)
//                {"edy7fq3L":"<matId>", "mnZ5K4Ii":"2"}, …] // evo mats   (role 2)
//   "I82p0wCL": [{"pn16CNah":"<targetMstId>"}]             // evolved-to unit MST id
//   "mCE3rUu5": [{"Rs7bCE3t":"<zelCost>"}]                 // zel cost
//
// Elem-item variant sends the base unit id under "8Z2NQrx1" instead of
// the role-1 entry inside "Km35HAXv"; both variants are handled here.
//
// Response:
//   "I82p0wCL": [EvoResultEntry]   — tells the client which MST id was evolved into
//   "qC2tJs4E": [UserUnitInfo]     — incremental unit cache update
//   "fEi17cnx": [UserTeamInfo]     — updated zel

// ---------------------------------------------------------------------------
// Request parsing structs
// ---------------------------------------------------------------------------
struct UnitEvoUnitEntry {
    int32_t     user_unit_id = 0;
    std::string role;
};
template <> struct glz::meta<UnitEvoUnitEntry> {
    using T = UnitEvoUnitEntry;
    static constexpr auto value = glz::object(
        "edy7fq3L", glz::quoted_num<&T::user_unit_id>,
        "mnZ5K4Ii", &T::role
    );
};

struct UnitEvoTargetEntry {
    int32_t target_mst_id = 0;
};
template <> struct glz::meta<UnitEvoTargetEntry> {
    using T = UnitEvoTargetEntry;
    static constexpr auto value = glz::object(
        "pn16CNah", glz::quoted_num<&T::target_mst_id>
    );
};

struct UnitEvoZelEntry {
    int32_t cost = 0;
};
template <> struct glz::meta<UnitEvoZelEntry> {
    using T = UnitEvoZelEntry;
    static constexpr auto value = glz::object(
        "Rs7bCE3t", glz::quoted_num<&T::cost>
    );
};

struct UnitEvoElemEntry {
    int32_t     user_unit_id = 0;
    std::string role;
};
template <> struct glz::meta<UnitEvoElemEntry> {
    using T = UnitEvoElemEntry;
    static constexpr auto value = glz::object(
        "inU8Q4gL", glz::quoted_num<&T::user_unit_id>,
        "mnZ5K4Ii", &T::role
    );
};

struct UnitEvoReqFull {
    std::vector<UnitEvoUnitEntry>   units;
    std::vector<UnitEvoTargetEntry> target;
    std::vector<UnitEvoZelEntry>    zel_cost_list;
    std::vector<UnitEvoElemEntry>   elem_units;
};
template <> struct glz::meta<UnitEvoReqFull> {
    using T = UnitEvoReqFull;
    static constexpr auto value = glz::object(
        "Km35HAXv", &T::units,
        "I82p0wCL", &T::target,
        "mCE3rUu5", &T::zel_cost_list,
        "8Z2NQrx1", &T::elem_units
    );
};

// ---------------------------------------------------------------------------
// Response structs
// ---------------------------------------------------------------------------
struct EvoResultEntry {
    int32_t evolved_unit_id = 0;  // pn16CNah — target MST id (what it evolved into)
    int32_t user_unit_id    = 0;  // edy7fq3L — DB instance id of the evolved unit
    int32_t unk_t9FEW2KC   = 0;  // t9FEW2KC — unknown, send 0
    int32_t unk_u1ECvfg8   = 0;  // u1ECvfg8 — unknown, send 0
    int32_t unk_dV3qji4I   = 0;  // dV3qji4I — unknown, send 0
};
template <> struct glz::meta<EvoResultEntry> {
    using T = EvoResultEntry;
    static constexpr auto value = glz::object(
        "pn16CNah", glz::quoted_num<&T::evolved_unit_id>,
        "edy7fq3L", glz::quoted_num<&T::user_unit_id>,
        "t9FEW2KC", glz::quoted_num<&T::unk_t9FEW2KC>,
        "u1ECvfg8", glz::quoted_num<&T::unk_u1ECvfg8>,
        "dV3qji4I", glz::quoted_num<&T::unk_dV3qji4I>
    );
};

// EvoReinforceEntry — xZH6EIQ7 payload (same wire format as MixReinforceEntry).
// The client's animation / stat-display code expects this key after every evo.
struct EvoReinforceEntry {
    std::string handle_name;
    int32_t     target_lv     = 0;
    std::string unit_mst_id;
    int32_t base_hp=0, base_atk=0, base_def=0, base_heal=0;
    int32_t add_hp=0,  add_atk=0,  add_def=0,  add_heal=0;
    int32_t ext_hp=0,  ext_atk=0,  ext_def=0;
    std::string skill_id, extra_skill_id;
    int32_t skill_lv=0, extra_skill_lv=0, unit_type_id=0;
    std::string mission_id;
};
template <> struct glz::meta<EvoReinforceEntry> {
    using T = EvoReinforceEntry;
    static constexpr auto value = glz::object(
        "B5JQyV8j", &T::handle_name,
        "4A6LzBxr", glz::quoted_num<&T::target_lv>,
        "pn16CNah", &T::unit_mst_id,
        "e7DK0FQT", glz::quoted_num<&T::base_hp>,
        "67CApcti", glz::quoted_num<&T::base_atk>,
        "q08xLEsy", glz::quoted_num<&T::base_def>,
        "PWXu25cg", glz::quoted_num<&T::base_heal>,
        "cuIWp89g", glz::quoted_num<&T::add_hp>,
        "RT4CtH5d", glz::quoted_num<&T::add_atk>,
        "GcMD0hy6", glz::quoted_num<&T::add_def>,
        "C1HZr3pb", glz::quoted_num<&T::add_heal>,
        "TokWs1B3", glz::quoted_num<&T::ext_hp>,
        "t4m1RH6Y", glz::quoted_num<&T::ext_atk>,
        "e6mY8Z0k", glz::quoted_num<&T::ext_def>,
        "nj9Lw7mV", &T::skill_id,
        "3NbeC8AB", glz::quoted_num<&T::skill_lv>,
        "iEFZ6H19", &T::extra_skill_id,
        "RQ5GnFE2", glz::quoted_num<&T::extra_skill_lv>,
        "nBTx56W9", glz::quoted_num<&T::unit_type_id>,
        "Ge8Yo32T", &T::mission_id
    );
};

struct UnitEvoRespBody {
    std::vector<EvoResultEntry>   evo_result;
    std::vector<UserUnitInfo>     unit_update;
    UserTeamInfo                  team_info = {};
    std::vector<EvoReinforceEntry> reinforce;
};
template <> struct glz::meta<UnitEvoRespBody> {
    using T = UnitEvoRespBody;
    static constexpr auto value = glz::object(
        "I82p0wCL", &T::evo_result,
        "qC2tJs4E", &T::unit_update,
        "fEi17cnx", pkg::glaze::single_array<&T::team_info>(),
        "xZH6EIQ7", &T::reinforce
    );
};

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------
static std::string unitEvo_addSuffix(int32_t mstId)
{
    return std::to_string(mstId) + "_100";
}

static UserTeamInfo unitEvo_buildTeamInfo(const drogon::orm::Row& row,
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
    ti.energy         = row["energy"].as<int32_t>();
    ti.max_action_point     = lv ? lv->energy    : 100;
    ti.deck_cost            = lv ? lv->deck_cost        : 20;
    ti.max_friend_count     = lv ? lv->friend_count     : 50;
    ti.add_friend_count     = lv ? lv->add_friend_count : 0;
    ti.max_unit_count       = row["max_unit_count"].as<int32_t>();
    ti.warehouse_count      = row["max_warehouse_count"].as<int32_t>();
    ti.active_deck          = row["active_deck"].as<int32_t>();
    ti.summon_ticket        = row["summon_tickets"].as<int32_t>();
    ti.rainbow_coin         = row["rainbow_coins"].as<int32_t>();
    ti.colosseum_ticket     = row["colosseum_tickets"].as<int32_t>();
    ti.friend_point         = row["friend_points"].as<int32_t>();
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

static std::string unitEvo_elementStr(int e)
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

// ---------------------------------------------------------------------------
// Handler
// ---------------------------------------------------------------------------
HANDLEF(UnitEvo)
{
    (void)session;
    LOG_INFO << "UnitEvo: " << json;

    // Transitional bridge: resolve the sole offline user at runtime
    // (tutorial-created).  TODO port to gme::getUserIdentity.
    const std::string kUserId = co_await gme::getSoleUserId(theDb());

    // Allow unknown keys — UnitEvo requests can carry extra client-side fields.
    UnitEvoReqFull req = {};
    {
        glz::context ctx{};
        if (const auto& ec = glz::read<glz::opts{.error_on_unknown_keys = false}>(req, json, ctx); ec)
        {
            LOG_WARN << "UnitEvo: bad request JSON: " << glz::format_error(ec, json);
            co_return HandleResult::error("Deserialization error");
        }
    }

    // Resolve base (role=1) and material (role=2) unit ids.
    int32_t baseId = 0;
    std::vector<int32_t> matIds;
    for (const auto& u : req.units)
    {
        if (u.role == "1")       baseId = u.user_unit_id;
        else if (u.user_unit_id) matIds.push_back(u.user_unit_id);
    }
    // Elem-item variant: 8Z2NQrx1 carries all units (base + mats) with role tags.
    // "inU8Q4gL" is the user_unit_id; "mnZ5K4Ii" is "1" (base) or "2" (material).
    for (const auto& e : req.elem_units)
    {
        if (e.role == "1")        baseId = e.user_unit_id;
        else if (e.user_unit_id)  matIds.push_back(e.user_unit_id);
    }

    if (baseId == 0 || req.target.empty())
    {
        LOG_WARN << "UnitEvo: missing base unit or target mst id";
        co_return HandleResult::error("UnitEvo: incomplete request");
    }

    const int32_t targetMstId = req.target[0].target_mst_id;
    const int32_t zelCost     = req.zel_cost_list.empty() ? 0 : req.zel_cost_list[0].cost;

    // Find target unit in MST cache.
    const auto& unitMst = theServer()->cache().unitMst();
    const UnitMst* targetMst = nullptr;
    for (const auto& u : unitMst) { if (u.id == targetMstId) { targetMst = &u; break; } }

    if (!targetMst)
    {
        LOG_WARN << "UnitEvo: target MST id " << targetMstId << " not found";
        co_return HandleResult::error("UnitEvo: unknown target unit");
    }

    // Step 1: SELECT current base unit (need IMP/ext/limitOver cols to preserve).
    // Also fetch unit_id so we can extract the original MST id for the animation.
    const auto baseRows = co_await theDb()->execSqlCoro(
        "SELECT id, unit_id, add_hp, add_atk, add_def, add_heal,"
        " ext_hp, ext_atk, ext_def, ext_heal,"
        " limit_over_hp, limit_over_atk, limit_over_def, limit_over_heal,"
        " fe_bp, fe_max_usable_bp, unit_type_id,"
        " eqip_item_id, eqip_item_frame_id, eqip_item_id2, eqip_item_frame_id2"
        " FROM user_units WHERE user_id=$1 AND user_unit_id=$2 LIMIT 1;",
        std::string(kUserId), baseId
    );

    if (baseRows.empty())
    {
        LOG_WARN << "UnitEvo: base unit " << baseId << " not found";
        co_return HandleResult::error("UnitEvo: base unit not found");
    }
    const auto& br = baseRows[0];

    // Extract original MST id from the unit_id column (e.g., "10011" or "10011_100" → 10011).
    // The client evo animation uses this to display the "before" unit.
    int32_t origMstId = 0;
    {
        const std::string rawUnitId = br["unit_id"].as<std::string>();
        const auto pos = rawUnitId.find('_');
        const std::string numPart = (pos != std::string::npos) ? rawUnitId.substr(0, pos) : rawUnitId;
        if (!numPart.empty()) { try { origMstId = std::stoi(numPart); } catch (...) {} }
    }

    // IMP stats are player investment — preserved through evolution.
    const int32_t keepAddHp   = br["add_hp"].as<int32_t>();
    const int32_t keepAddAtk  = br["add_atk"].as<int32_t>();
    const int32_t keepAddDef  = br["add_def"].as<int32_t>();
    const int32_t keepAddHeal = br["add_heal"].as<int32_t>();

    const std::string newElement = unitEvo_elementStr(targetMst->element);

    // Step 2: UPDATE base unit row to the evolved form.
    //   - New unit_id, reset level/exp to 1/0, reset base stats from target MST.
    //   - Preserve: add_* (IMP), ext_*, limit_over_*, equipment, FE.
    co_await theDb()->execSqlCoro(
        "UPDATE user_units SET"
        " unit_id=$1,"
        " unit_lv=1, exp=0, total_exp=0,"
        " base_hp=$2,  base_atk=$3,  base_def=$4,  base_heal=$5,"
        " add_hp=$6,   add_atk=$7,   add_def=$8,   add_heal=$9,"
        " leader_skill_id=$10, skill_id=$11, extra_skill_id=$12,"
        " skill_lv=1, extra_skill_lv=0,"
        " element=$13"
        " WHERE user_unit_id=$14 AND user_id=$15;",
        unitEvo_addSuffix(targetMstId),
        targetMst->min_hp,  targetMst->min_atk,  targetMst->min_def,  targetMst->min_rec,
        keepAddHp,           keepAddAtk,           keepAddDef,          keepAddHeal,
        targetMst->leader_skill_id, targetMst->skill_id, targetMst->extra_skill_id,
        newElement,
        baseId, std::string(kUserId)
    );

    // Step 3: DELETE evo material units.
    if (!matIds.empty())
    {
        std::string matList;
        for (size_t i = 0; i < matIds.size(); ++i)
        {
            if (i) matList += ',';
            matList += std::to_string(matIds[i]);
        }
        co_await theDb()->execSqlCoro(
            "DELETE FROM user_units WHERE user_id=$1 AND user_unit_id IN (" + matList + ");",
            std::string(kUserId)
        );
    }

    // Step 4: deduct zel.
    if (zelCost > 0)
    {
        co_await theDb()->execSqlCoro(
            "UPDATE user_info SET zel = MAX(0, zel - $1) WHERE id=$2;",
            zelCost, std::string(kUserId)
        );
    }

    // Step 5: fresh user_info for team_info.
    const auto infoRows = co_await theDb()->execSqlCoro(
        "SELECT level, exp, zel, karma, brave_coin, 0 AS free_gems, gems AS paid_gems, energy,"
        " max_unit_count, max_warehouse_count, summon_tickets, rainbow_coins,"
        " colosseum_tickets, friend_points, total_brave_points, avail_brave_points,"
        " active_deck, want_gift FROM user_info WHERE id=$1;",
        std::string(kUserId)
    );

    // Build response.
    UnitEvoRespBody resp = {};

    {
        EvoResultEntry er = {};
        er.evolved_unit_id = targetMstId;   // pn16CNah — the unit it evolved INTO
        er.user_unit_id    = baseId;         // edy7fq3L — DB instance id
        er.unk_t9FEW2KC   = origMstId;       // t9FEW2KC — original ("before") MST id for evo animation
        resp.evo_result.emplace_back(er);
    }

    {
        UserUnitInfo ud = {};
        ud.user_id             = std::string(kUserId);
        ud.user_unit_id        = br["id"].as<int32_t>();
        ud.unit_id             = targetMstId;
        ud.unit_type_id        = br["unit_type_id"].as<int32_t>();
        ud.unit_lvl             = 1;
        ud.exp                 = 0;
        ud.total_exp           = 0;
        ud.base_hp             = targetMst->min_hp;
        ud.add_hp              = keepAddHp;
        ud.ext_hp              = br["ext_hp"].as<int32_t>();
        ud.limit_over_hp       = br["limit_over_hp"].as<int32_t>();
        ud.base_atk            = targetMst->min_atk;
        ud.add_atk             = keepAddAtk;
        ud.ext_atk             = br["ext_atk"].as<int32_t>();
        ud.limit_over_atk      = br["limit_over_atk"].as<int32_t>();
        ud.base_def            = targetMst->min_def;
        ud.add_def             = keepAddDef;
        ud.ext_def             = br["ext_def"].as<int32_t>();
        ud.limit_over_def      = br["limit_over_def"].as<int32_t>();
        ud.base_rec           = targetMst->min_rec;
        ud.add_rec            = keepAddHeal;
        ud.ext_rec            = br["ext_heal"].as<int32_t>();
        ud.limit_over_rec     = br["limit_over_heal"].as<int32_t>();
        ud.element             = newElement;
        ud.leader_skill_id     = targetMst->leader_skill_id;
        ud.bb_id            = std::to_string(targetMst->skill_id);
        ud.bb_lvl            = 1;
        ud.sbb_id      = std::to_string(targetMst->extra_skill_id);
        ud.sbb_lvl      = 0;
        ud.equipitem_id        = br["eqip_item_id"].as<int32_t>();
        ud.equipitem_frame_id  = br["eqip_item_frame_id"].as<int32_t>();
        ud.equipitem_id2       = br["eqip_item_id2"].as<int32_t>();
        ud.equipitem_frame_id2 = br["eqip_item_frame_id2"].as<int32_t>();
        ud.fe_bp               = br["fe_bp"].as<int32_t>();
        ud.fe_max_usable_bp    = br["fe_max_usable_bp"].as<int32_t>();
        ud.is_new            = true;
        resp.unit_update.emplace_back(std::move(ud));
    }

    resp.team_info = unitEvo_buildTeamInfo(
        infoRows.at(0),
        theServer()->cache().initializeResp().progression
    );

    {
        EvoReinforceEntry rd = {};
        rd.handle_name    = "DecompDev";
        rd.target_lv      = 1;  // level resets to 1 after evo
        rd.unit_mst_id    = std::to_string(targetMstId);
        rd.base_hp        = targetMst->min_hp;
        rd.base_atk       = targetMst->min_atk;
        rd.base_def       = targetMst->min_def;
        rd.base_heal      = targetMst->min_rec;
        rd.add_hp         = keepAddHp;
        rd.add_atk        = keepAddAtk;
        rd.add_def        = keepAddDef;
        rd.add_heal       = keepAddHeal;
        rd.ext_hp         = br["ext_hp"].as<int32_t>();
        rd.ext_atk        = br["ext_atk"].as<int32_t>();
        rd.ext_def        = br["ext_def"].as<int32_t>();
        rd.skill_id       = std::to_string(targetMst->skill_id);
        rd.skill_lv       = 1;
        rd.extra_skill_id = std::to_string(targetMst->extra_skill_id);
        rd.extra_skill_lv = 0;
        rd.unit_type_id   = br["unit_type_id"].as<int32_t>();
        rd.mission_id     = "";
        resp.reinforce.emplace_back(std::move(rd));
    }

    std::string buffer{};
    if (const auto& ec2 = glz::write_json(resp, buffer); ec2)
    {
        LOG_ERROR << "UnitEvo: serialization error: " << glz::format_error(ec2, buffer);
        co_return HandleResult::error("Serialization error");
    }

    co_return HandleResult::success(buffer);
}
