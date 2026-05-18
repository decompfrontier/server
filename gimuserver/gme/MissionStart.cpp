#include "App.hpp"
#include "Handlers.hpp"

#include <ctime>

// MissionStart (jE6Sp0q4) — fired when the player taps "Quest" and selects
// a mission.  Returns the full battle-engine seed: 5 enemy waves with monster
// stats/skills/AI, plus the player's UserTeamInfo and misc session keys.
// All enemy data is taken from the original live-server test mission (ID 10).

// ── Battle-engine MST structs ────────────────────────────────────────────────

struct MsBattleGroup {
    uint32_t group_id = 0, mission_id = 0, battle_order = 0,
             first_atk_rate = 0, battle_monster_id = 0, boss_flg = 0;
};
template<> struct glz::meta<MsBattleGroup> {
    using T = MsBattleGroup;
    static constexpr auto value = glz::object(
        "ZSf8e1MG", glz::quoted_num<&T::group_id>,
        "j28VNcUW", glz::quoted_num<&T::mission_id>,
        "VETu07N6", glz::quoted_num<&T::battle_order>,
        "5aetPz3C", glz::quoted_num<&T::first_atk_rate>,
        "etM5TCb9", glz::quoted_num<&T::boss_flg>,
        "Qzhp8B40", glz::quoted_num<&T::battle_monster_id>
    );
};

struct MsBattleMonsterGroup {
    uint32_t battle_monster_id = 0, monster_id = 0, group_order = 0;
    std::string position, item_drop, unit_drop, treasure_drop;
};
template<> struct glz::meta<MsBattleMonsterGroup> {
    using T = MsBattleMonsterGroup;
    static constexpr auto value = glz::object(
        "Qzhp8B40", glz::quoted_num<&T::battle_monster_id>,
        "o49dYfpH", glz::quoted_num<&T::monster_id>,
        "hZtF1s8B", glz::quoted_num<&T::group_order>,
        "3g8PW6x0", &T::position,
        "i30R8TAs", &T::item_drop,
        "hw3L0uVj", &T::unit_drop,
        "5wB9SHAV", &T::treasure_drop
    );
};

struct MsMonster {
    uint32_t monster_id = 0, hp = 0, atk = 0, def = 0, element = 0,
             drop_check_cnt = 0, max_zel_drop = 0, zel_drop_cnt = 0,
             max_karma_drop = 0, karma_drop_cnt = 0, wait = 0,
             move_speed_type = 0, atk_move_type = 0, back_move_type = 0,
             skill_move_type = 0, after_image = 0, max_act_cnt = 0,
             min_act_cnt = 0, ai_id = 0, unit_id = 0;
    int32_t  act_rate = 0;
    std::string bad_state_resists = "0:0:0:0:0:0",
                effect_frame, damage_frame,
                move_offset = "0,0", lkh_val = "0,0",
                debuff_resist = "0,0,0", unit_skills,
                e1, e2, e3, e4, e5, e6, e7, e8, e9;
};
template<> struct glz::meta<MsMonster> {
    using T = MsMonster;
    static constexpr auto value = glz::object(
        "o49dYfpH", glz::quoted_num<&T::monster_id>,
        "e7DK0FQT", glz::quoted_num<&T::hp>,
        "67CApcti", glz::quoted_num<&T::atk>,
        "q08xLEsy", glz::quoted_num<&T::def>,
        "CEeqs63b", &T::bad_state_resists,
        "iNy0ZU5M", glz::quoted_num<&T::element>,
        "eyUo6a8c", &T::effect_frame,
        "6Aou5M9r", &T::damage_frame,
        "n9h7p02P", glz::quoted_num<&T::drop_check_cnt>,
        "Najhr8m6", glz::quoted_num<&T::max_zel_drop>,
        "9FN0GAei", glz::quoted_num<&T::zel_drop_cnt>,
        "HTVh8a65", glz::quoted_num<&T::max_karma_drop>,
        "vNk2sI4X", glz::quoted_num<&T::karma_drop_cnt>,
        "9pXSKmn1", glz::quoted_num<&T::wait>,
        "J2hPXGo5", glz::quoted_num<&T::move_speed_type>,
        "mv4o39Uz", glz::quoted_num<&T::atk_move_type>,
        "h2L1YI90", glz::quoted_num<&T::back_move_type>,
        "XE7Yi5c3", glz::quoted_num<&T::skill_move_type>,
        "D4Y5bWK7", glz::quoted_num<&T::after_image>,
        "oMGC3hW0", glz::quoted_num<&T::max_act_cnt>,
        "m4EK7Gt6", glz::quoted_num<&T::min_act_cnt>,
        "6fwL59FT", glz::quoted_num<&T::act_rate>,
        "i74vGUFa", glz::quoted_num<&T::ai_id>,
        "CYk84E3W", &T::move_offset,
        "Lkh6gYkT", &T::lkh_val,
        "pn16CNah", glz::quoted_num<&T::unit_id>,
        "Y6bd4fXp", &T::debuff_resist,
        "F4bQ7r8C", &T::unit_skills,
        "Btf93Xs1", &T::e1,
        "3evIn0zZ", &T::e2,
        "hjAy9St3", &T::e3,
        "2Smu5Mtq", &T::e4,
        "2EF0d6ue", &T::e5,
        "QqfI9mM4", &T::e6,
        "3BpHN6VD", &T::e7,
        "jm6JSK2D", &T::e8,
        "qp37xTDh", &T::e9
    );
};

struct MsUnitSkill {
    uint32_t skill_id = 0, skill_type = 0, skill_rank = 0,
             move_flag = 0, atk_move_flag = 0, drop_check_cnt = 0,
             wait = 0, element = 0;
    std::string skill_name, process_id,
                target_type = "1", target_area = "0",
                disp_frame = "5:18", effect_frame = "5:18",
                start_frame = "5", damage_frame = "18:1",
                e1, e2 = "0", e3 = "0", e4 = ",,";
};
template<> struct glz::meta<MsUnitSkill> {
    using T = MsUnitSkill;
    static constexpr auto value = glz::object(
        "nj9Lw7mV", glz::quoted_num<&T::skill_id>,
        "0nxpBDz2", &T::skill_name,
        "h6UL9A1B", glz::quoted_num<&T::skill_type>,
        "AR7y0sY4", glz::quoted_num<&T::skill_rank>,
        "hjAy9St3", &T::process_id,
        "moWQ30GH", &T::target_type,
        "IZt73kLG", &T::target_area,
        "I7cmtqW8", glz::quoted_num<&T::move_flag>,
        "K2i8emcH", glz::quoted_num<&T::atk_move_flag>,
        "diGe6u21", &T::disp_frame,
        "eyUo6a8c", &T::effect_frame,
        "qYCx73y2", &T::start_frame,
        "6Aou5M9r", &T::damage_frame,
        "n9h7p02P", glz::quoted_num<&T::drop_check_cnt>,
        "9pXSKmn1", glz::quoted_num<&T::wait>,
        "iNy0ZU5M", glz::quoted_num<&T::element>,
        "qp37xTDh", &T::e1,
        "ToZj4zyn", &T::e2,
        "57BsydKH", &T::e3,
        "R8uaKdbf", &T::e4
    );
};

struct MsAI {
    uint32_t ai_id = 0, priority = 0, target = 0, percent = 0;
    std::string ai_term, search_term, atk_param;
};
template<> struct glz::meta<MsAI> {
    using T = MsAI;
    static constexpr auto value = glz::object(
        "4eEVw5hL", glz::quoted_num<&T::ai_id>,
        "yu18xScw", glz::quoted_num<&T::priority>,
        "q7Nit8JW", &T::ai_term,
        "4xctV8gF", glz::quoted_num<&T::target>,
        "VBj9u0ot", &T::search_term,
        "Hhgi79M1", &T::atk_param,
        "ug9xV4Fz", glz::quoted_num<&T::percent>
    );
};

// Container for all battle-engine arrays (serialized together, then spliced in)
struct MsBattleData {
    std::vector<MsBattleGroup>        battle_groups;
    std::vector<MsBattleMonsterGroup> monster_groups;
    std::vector<MsMonster>            monsters;
    std::vector<MsUnitSkill>          skills;
    std::vector<MsAI>                 ai;
};
template<> struct glz::meta<MsBattleData> {
    using T = MsBattleData;
    static constexpr auto value = glz::object(
        "pj41dy9g", &T::battle_groups,
        "75t0sx9z", &T::monster_groups,
        "U0v5IeJo", &T::monsters,
        "8aiBoHg5", &T::skills,
        "89ausgc4", &T::ai
    );
};

// Wraps UserTeamInfo under the "fEi17cnx" single-element array key
struct MsTeamWrapper {
    UserTeamInfo team_info = {};
};
template<> struct glz::meta<MsTeamWrapper> {
    using T = MsTeamWrapper;
    static constexpr auto value = glz::object(
        "fEi17cnx", pkg::glaze::single_array<&T::team_info>()
    );
};

// ── DB helper ────────────────────────────────────────────────────────────────

static UserTeamInfo missionStart_buildTeamInfo(
    const drogon::orm::Row& row,
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

// ── Handler ──────────────────────────────────────────────────────────────────
//
// Request shape is generated from packet-generator/assets/net/mission.kdl
// (MissionStartReq / MissionStartReqItem) — declares all 8 fields the client
// sends inside 9Q1Lq5FS[0], so glaze's lenient parse no longer has to skip
// 6 unknown sibling keys and silently fail on Z0Y4RoD7 the way it did when
// only mission_id + active_deck were declared locally.
//
// Earlier drafts placed mission_id at the top level — that silently failed
// to parse on every request and every mission was answered with mission-10
// data; see handbook §7.4.1.  The substring-fallback for Z0Y4RoD7 added
// during the team-loading debugging is now redundant with the full struct
// declaration but is kept as a defensive belt-and-braces.

HANDLEF(MissionStart)
{
    LOG_INFO << "MissionStart: " << json;

    static constexpr std::string_view kUserId = "0839899613932562";

    // Parse using the generated struct (handles all 8 inner-item keys).
    ::MissionStartReq req{};
    if (const auto ec = glz::read<glz::opts{.error_on_unknown_keys = false}>(req, json); ec)
        LOG_WARN << "MissionStart: parse warning: " << glz::format_error(ec, json);

    // Generated mission_id is i32::str → parsed as int32_t.  Convert to a
    // string view for the existing response-building path that emits the
    // mission_id back to the client as a quoted JSON value.
    std::string missionIdStr = std::to_string(req.items.mission_id);
    if (req.items.mission_id == 0)
        missionIdStr = "10";   // defensive fallback for malformed requests

    int32_t reqActiveDeck = req.items.active_deck;

    // Belt-and-braces substring extraction kept from the team-loading fix —
    // historically glaze silently returned 0 here when only 2 of 8 keys
    // were declared; with the generated struct declaring all 8 it shouldn't
    // be needed, but leaving the safety net costs us nothing.
    if (reqActiveDeck == 0)
    {
        const std::string_view needle = "\"Z0Y4RoD7\":\"";
        const auto pos = json.find(needle);
        if (pos != std::string::npos)
        {
            const auto valStart = pos + needle.size();
            const auto valEnd   = json.find('"', valStart);
            if (valEnd != std::string::npos && valEnd > valStart)
            {
                try { reqActiveDeck = std::stoi(std::string(json.substr(valStart, valEnd - valStart))); }
                catch (...) { /* leave at 0 */ }
            }
        }
    }

    // Parse mission_id as integer for battle-group struct fields.  Defaults to
    // 10 if the string is non-numeric (event missions with non-numeric IDs).
    uint32_t missionIdNum = 10;
    try { missionIdNum = static_cast<uint32_t>(std::stoul(missionIdStr)); }
    catch (...) { LOG_WARN << "MissionStart: mission_id '" << missionIdStr << "' not numeric, using 10"; }

    LOG_INFO << "MissionStart: active_deck=" << reqActiveDeck
             << " mission_id=" << missionIdStr << " (num=" << missionIdNum << ")";

    // ── DB writes: deduct energy + persist active deck selection ─────────────
    //
    // Look up the per-mission energy cost from F_MISSION_MST (cached at
    // boot from deploy/system/mission_mst.json, wrapper key oXeC1Ak9 — see
    // ServerCache::missionMst()).  Field 69vnphig on each MissionMst row
    // is the stamina cost.  Tutorial / early-chapter missions cost 3;
    // endgame missions can be 30+.  Falls back to 10 (the old hardcoded
    // value) if the mission_id isn't present in the cache, so handlers
    // for synthetic / event mission IDs not in F_MISSION_MST still
    // deduct *something*.
    int32_t energyCost = 10;
    {
        const auto& missionMst = theServer()->cache().missionMst();
        for (const auto& m : missionMst)
        {
            if (m.id == static_cast<int32_t>(missionIdNum))
            {
                energyCost = m.stamina_cost;
                break;
            }
        }
    }
    LOG_INFO << "MissionStart: energy cost for mission_id=" << missionIdStr
             << " = " << energyCost
             << " (from F_MISSION_MST.69vnphig)";

    try
    {
        co_await theDb()->execSqlCoro(
            "UPDATE userinfo SET energy = MAX(0, energy - $1) WHERE id=$2;",
            energyCost, std::string(kUserId));
    }
    catch (const drogon::orm::DrogonDbException& ex)
    {
        LOG_WARN << "MissionStart: energy UPDATE failed: " << ex.base().what();
    }

    try
    {
        co_await theDb()->execSqlCoro(
            "UPDATE userinfo SET active_deck = $1 WHERE id=$2;",
            reqActiveDeck, std::string(kUserId));
    }
    catch (const drogon::orm::DrogonDbException& ex)
    {
        LOG_WARN << "MissionStart: active_deck UPDATE failed: " << ex.base().what();
    }

    const auto infoRows = co_await theDb()->execSqlCoro(
        "SELECT level, exp, zel, karma, brave_coin, free_gems, paid_gems, energy,"
        " max_unit_count, max_warehouse_count, summon_tickets, rainbow_coins,"
        " colosseum_tickets, friend_point, total_brave_points, avail_brave_points,"
        " active_deck, want_gift, username FROM userinfo WHERE id=$1;",
        std::string(kUserId));

    if (infoRows.empty())
    {
        LOG_ERROR << "MissionStart: userinfo not found";
        co_return HandleResult::success("{}");
    }

    const auto& row      = infoRows[0];
    const auto  username = row["username"].as<std::string>();
    const int32_t activeDeck = row["active_deck"].as<int32_t>();

    // ── fEi17cnx[0] — player UserTeamInfo ───────────────────────────────────
    MsTeamWrapper tw{};
    tw.team_info = missionStart_buildTeamInfo(
        row, theServer()->cache().initializeResp().progression);

    std::string twJson{};
    if (const auto ec = glz::write_json(tw, twJson); ec)
    {
        LOG_ERROR << "MissionStart: serialize teamInfo: " << glz::format_error(ec, twJson);
        co_return HandleResult::success("{}");
    }
    // twJson = {"fEi17cnx":[{...player stats...}]}  ends with ]}

    // ── Battle-engine MST data ───────────────────────────────────────────────
    MsBattleData bd{};

    // TODO: load real per-mission battle waves from F_MISSION_MST +
    // F_BATTLE_GROUP_MST + F_MONSTER_MST (decode the encrypted .dat files
    // in deploy/game_content/mst/ via scripts/mstdec.py into deploy/system/
    // JSON, then read at boot in ServerCache.cpp).  Currently every mission
    // returns the SAME enemy waves (mission-10's captured live data) — the
    // mission_id header is dynamic so the client renders without crashing,
    // but the boss, monsters, and rewards are always mission-10's.

    // BattleGroupMst — wave ordering.  The mission_id field (second column)
    // MUST match the request's mission_id, otherwise the client treats the
    // response as belonging to a different mission and crashes on load.
    bd.battle_groups = {
        {11, missionIdNum, 1, 0, 101301, 0},
        {12, missionIdNum, 2, 0, 101302, 0},
        {14, missionIdNum, 3, 0, 101300, 0},
        {16, missionIdNum, 4, 0, 101302, 0},
        {18, missionIdNum, 5, 0, 101304, 1},
    };

    // BattleMonsterGroupMst — enemy placement per wave
    bd.monster_groups = {
        {101301, 30352, 0, "180:302", "7:10000:3,7:10300:3", "25:30030:1:0,30:30030:2:0", "15,25:500,25:250,25:10,25:10000:1"},
        {101301, 40352, 1, "120:248", "7:10000:3,7:10300:3", "25:40030:1:0,30:40030:2:0", "15,25:500,25:250,25:10,25:10000:1"},
        {101302, 10352, 0, "180:302", "7:10000:3,7:10300:3", "25:10030:1:0,30:10030:2:0", "15,25:500,25:250,25:10,25:10000:1"},
        {101302, 50352, 1, "120:248", "7:10000:3,7:10300:3", "25:50030:1:0,30:50030:2:0", "15,25:500,25:250,25:10,25:10000:1"},
        {101302, 30352, 2, "96:352",  "7:10000:3,7:10300:3", "25:30030:1:0,30:30030:2:0", "15,25:500,25:250,25:10,25:10000:1"},
        {101300, 10352, 0, "180:302", "7:10000:3,7:10300:3", "25:10030:1:0,30:10030:2:0", "15,25:500,25:250,25:10,25:10000:1"},
        {101300, 20352, 1, "120:248", "7:10000:3,7:10300:3", "25:20030:1:0,30:20030:2:0", "15,25:500,25:250,25:10,25:10000:1"},
        {101304, 40401, 0, "180:302", "7:10301:5,3:10603:1",  "0:0:0:0,0:0:0:0",           "0,25:500,25:250,25:10,25:10301:1"},
    };

    // MonsterMst — enemy stat definitions
    auto mkm = [](uint32_t id, uint32_t hp, uint32_t atk, uint32_t def, uint32_t elem,
                  uint32_t wait, const char* efx, const char* dmg,
                  uint32_t maxZel, uint32_t zelCnt,
                  uint32_t maxKarma, uint32_t karmaCnt,
                  uint32_t aiId, uint32_t unitId) -> MsMonster {
        MsMonster m{};
        m.monster_id      = id;
        m.hp              = hp;
        m.atk             = atk;
        m.def             = def;
        m.element         = elem;
        m.drop_check_cnt  = 1;
        m.max_zel_drop    = maxZel;
        m.zel_drop_cnt    = zelCnt;
        m.max_karma_drop  = maxKarma;
        m.karma_drop_cnt  = karmaCnt;
        m.wait            = wait;
        m.move_speed_type = 2;
        m.atk_move_type   = 1;
        m.back_move_type  = 1;
        m.skill_move_type = 1;
        m.after_image     = 1;
        m.max_act_cnt     = 1;
        m.min_act_cnt     = 1;
        m.act_rate        = 100;
        m.ai_id           = aiId;
        m.unit_id         = unitId;
        m.effect_frame    = efx;
        m.damage_frame    = dmg;
        return m;
    };
    bd.monsters = {
        mkm(30352, 830,  340,  90, 3, 5, "24:123:1", "30:100:2:1", 33, 5, 21, 5, 1, 30030),
        mkm(40352, 760,  370,   0, 4, 2, "24:124:1", "30:100:2:1", 30, 5, 19, 5, 1, 40030),
        mkm(10352, 800,  320,  50, 1, 5, "24:121:1", "30:100:2:1", 32, 5, 20, 5, 1, 10030),
        mkm(50352, 850,  340,  60, 5, 1, "24:125:1", "30:100:2:1", 34, 5, 21, 5, 1, 50030),
        mkm(20352, 780,  330,  20, 2, 5, "24:122:1", "30:100:2:1", 31, 5, 20, 5, 1, 20030),
        mkm(40401, 2500, 470,  30, 4, 0, "36:124:1", "42:100:2:1", 100, 5, 63, 5, 1, 40031),
    };

    // UnitSkillMst — skill definitions used by the enemy AI
    auto mks = [](uint32_t id, uint32_t type, uint32_t rank, const char* proc,
                  uint32_t dropCnt = 0) -> MsUnitSkill {
        MsUnitSkill s{};
        s.skill_id       = id;
        s.skill_type     = type;
        s.skill_rank     = rank;
        s.process_id     = proc;
        s.drop_check_cnt = dropCnt;
        return s;
    };
    bd.skills = {
        mks(2000140, 1, 1, "1:10000:1"),
        mks(2000141, 1, 1, "1:10300:1"),
        mks(2000142, 1, 1, "1:10000:1"),
        mks(2000143, 1, 1, "1:10300:1"),
        mks(2000144, 1, 1, "1:10000:1"),
        mks(2000145, 1, 1, "1:10300:1"),
        mks(2000146, 1, 1, "1:10301:5"),
        mks(2000147, 3, 1, "3:10603:1"),
        mks(2000148, 1, 1, "1:10000:1"),
    };

    // Include player unit BB skills so the battle engine can fill gauges.
    // Query the active deck's units and add a skill entry for each unique
    // BB skill_id.  process_id="1" and drop_check_cnt=1 match the values
    // found in F_SKILL_MST for all regular player BB skills.
    try
    {
        const auto deckSkillRows = co_await theDb()->execSqlCoro(
            "SELECT DISTINCT uu.skill_id"
            " FROM user_party_decks pd"
            " JOIN user_units uu ON uu.id = pd.user_unit_id"
            " WHERE pd.user_id=$1 AND pd.deck_num=$2 AND uu.skill_id > 0",
            std::string(kUserId), activeDeck);

        for (const auto& r : deckSkillRows)
        {
            const auto skillId = r["skill_id"].as<uint32_t>();
            // Skip if already listed (no duplicate entries).
            bool found = false;
            for (const auto& s : bd.skills) { if (s.skill_id == skillId) { found = true; break; } }
            if (!found)
                bd.skills.emplace_back(mks(skillId, 1, 1, "1", 1));
        }
    }
    catch (const drogon::orm::DrogonDbException& ex)
    {
        LOG_WARN << "MissionStart: deck skill query failed: " << ex.base().what();
    }

    // ── Reinforce slot — tojMy68W (FriendInfoResponse).  The squad-select
    // friend picker reads from the client's FriendInfoList singleton; that
    // singleton is populated by the tojMy68W readParam (see audit at
    // tools/ida/audits/tojMy68W_audit.txt, handler at 0x13D93C0 in
    // libgame.so).  Previous attempts emitted xZH6EIQ7 (ReinforcementInfo)
    // and T_FIXED_REINFORCEMENT (FixedReinforcementInfo) which feed
    // different singletons that the squad-select UI doesn't appear to
    // consult — those were correctly received by the binary but landed in
    // lists nothing was reading.
    //
    // We don't have a real friend system on an offline server, so the
    // entry stands in for the player's own active-deck leader (or their
    // highest-level unit as a fallback).  TODO: once a curated set of
    // pre-made friends exists, emit an array of FriendInfo entries
    // instead of just one.
    //
    // KDL schema is in packet-generator/assets/net/friends.kdl.
    FriendInfo friend_entry{};
    bool       haveFriend = false;
    try
    {
        auto reinforceRows = co_await theDb()->execSqlCoro(
            "SELECT uu.id, uu.unit_id, uu.unit_lv,"
            " uu.base_hp,  uu.add_hp,  uu.ext_hp,"
            " uu.base_atk, uu.add_atk, uu.ext_atk,"
            " uu.base_def, uu.add_def, uu.ext_def,"
            " uu.base_heal,uu.add_heal,uu.ext_heal,"
            " uu.skill_id, uu.skill_lv, uu.extra_skill_id, uu.extra_skill_lv,"
            " uu.unit_type_id, uu.element"
            " FROM user_party_decks pd"
            " JOIN user_units uu ON uu.id = pd.user_unit_id"
            " WHERE pd.user_id=$1 AND pd.deck_num=$2 AND pd.member_type=0"
            " LIMIT 1;",
            std::string(kUserId), activeDeck);

        if (reinforceRows.empty())
        {
            // No active-deck leader row — fall back to the player's
            // highest-level unit so the slot still has data.
            reinforceRows = co_await theDb()->execSqlCoro(
                "SELECT id, unit_id, unit_lv,"
                " base_hp,  add_hp,  ext_hp,"
                " base_atk, add_atk, ext_atk,"
                " base_def, add_def, ext_def,"
                " base_heal,add_heal,ext_heal,"
                " skill_id, skill_lv, extra_skill_id, extra_skill_lv,"
                " unit_type_id, element"
                " FROM user_units"
                " WHERE user_id=$1"
                " ORDER BY unit_lv DESC, id DESC LIMIT 1;",
                std::string(kUserId));
        }

        if (!reinforceRows.empty())
        {
            const auto& r = reinforceRows[0];

            // user_units.unit_id is TEXT and may carry a "_100" suffix on
            // evolved units.  Strip it for the MST id we put on the wire.
            const auto raw = r["unit_id"].as<std::string>();
            const auto sep = raw.find('_');
            int32_t mstId = 0;
            try { mstId = std::stoi(sep != std::string::npos ? raw.substr(0, sep) : raw); }
            catch (...) {}

            // FriendInfo's element field is uint32_t (int element id) — NOT
            // a string like UserUnitInfo.element.  user_units.element stores
            // the string form ("fire"/"water"/etc); invert for the wire.
            const auto elemStr = r["element"].as<std::string>();
            int32_t elemId = 1;  // default fire
            if      (elemStr == "fire")    elemId = 1;
            else if (elemStr == "water")   elemId = 2;
            else if (elemStr == "earth")   elemId = 3;
            else if (elemStr == "thunder") elemId = 4;
            else if (elemStr == "light")   elemId = 5;
            else if (elemStr == "dark")    elemId = 6;

            // user_id "n9ZMPC0t" matches the placeholder friend already used
            // in zI2tJB7R above; keeps the squad-selection UI internally
            // consistent.
            friend_entry.user_id            = "n9ZMPC0t";
            friend_entry.handle_name        = "DecompFriend";
            friend_entry.team_lv            = 999;  // placeholder account level
            friend_entry.friend_type        = 1;    // 1 = friend (UNVERIFIED enum)
            friend_entry.last_login_date    = static_cast<int32_t>(std::time(nullptr));
            friend_entry.unit_id            = mstId;
            friend_entry.unit_lv            = r["unit_lv"].as<int32_t>();
            friend_entry.base_hp            = r["base_hp"].as<int32_t>();
            friend_entry.add_hp             = r["add_hp"].as<int32_t>();
            friend_entry.ext_hp             = r["ext_hp"].as<int32_t>();
            friend_entry.base_atk           = r["base_atk"].as<int32_t>();
            friend_entry.add_atk            = r["add_atk"].as<int32_t>();
            friend_entry.ext_atk            = r["ext_atk"].as<int32_t>();
            friend_entry.base_def           = r["base_def"].as<int32_t>();
            friend_entry.add_def            = r["add_def"].as<int32_t>();
            friend_entry.ext_def            = r["ext_def"].as<int32_t>();
            friend_entry.base_heal          = r["base_heal"].as<int32_t>();
            friend_entry.add_heal           = r["add_heal"].as<int32_t>();
            friend_entry.ext_heal           = r["ext_heal"].as<int32_t>();
            friend_entry.skill_id           = std::to_string(r["skill_id"].as<int32_t>());
            friend_entry.skill_lv           = r["skill_lv"].as<int32_t>();
            friend_entry.extra_skill_id     = std::to_string(r["extra_skill_id"].as<int32_t>());
            friend_entry.extra_skill_lv    = r["extra_skill_lv"].as<int32_t>();
            friend_entry.unit_type_id       = r["unit_type_id"].as<int32_t>();
            friend_entry.element            = elemId;
            friend_entry.friend_id          = "DECOMP01";       // displayable in-game ID
            friend_entry.friend_message     = "GG WP";          // profile/status string
            friend_entry.favorite           = 1;                // mark as favorite so it sorts to the top
            friend_entry.priority           = 1;                // ditto
            friend_entry.deck_no            = 0;
            friend_entry.guild_id           = 0;
            // All other unk_* / arena fields default to 0/"" — that matches
            // the bfdata typed handler's initial values.

            haveFriend = true;
            LOG_INFO << "MissionStart: tojMy68W friend slot populated from unit "
                     << mstId << " (lv " << friend_entry.unit_lv << ")";
        }
        else
        {
            LOG_WARN << "MissionStart: no units in inventory — friend slot will be empty";
        }
    }
    catch (const drogon::orm::DrogonDbException& ex)
    {
        LOG_WARN << "MissionStart: friend query failed: " << ex.base().what();
    }

    // AIMst — enemy AI behaviour (ai_id 20017)
    auto mkai = [](uint32_t pri, const char* term, uint32_t tgt,
                   const char* search, const char* atk, uint32_t pct) -> MsAI {
        MsAI a{};
        a.ai_id       = 20017;
        a.priority    = pri;
        a.ai_term     = term;
        a.target      = tgt;
        a.search_term = search;
        a.atk_param   = atk;
        a.percent     = pct;
        return a;
    };
    bd.ai = {
        mkai(1,  "1:50:1", 1, "1:1", "2000140", 100),
        mkai(2,  "1:50:2", 1, "1:1", "2000141", 100),
        mkai(3,  "1:50:3", 1, "1:1", "2000142", 100),
        mkai(4,  "1:50:4", 1, "1:1", "2000143", 100),
        mkai(5,  "1:50:5", 1, "1:1", "2000144", 100),
        mkai(6,  "0:0:0",  1, "1:1", "2000140",  50),
        mkai(7,  "0:0:0",  1, "2:1", "2000141",  50),
        mkai(8,  "0:0:0",  1, "1:1", "2000142",  50),
        mkai(9,  "0:0:0",  1, "2:1", "2000143",  50),
        mkai(10, "0:0:0",  1, "1:1", "2000144", 100),
        mkai(11, "0:0:0",  1, "1:1", "2000146",  60),
        mkai(12, "0:0:0",  1, "1:1", "2000147",  40),
    };

    std::string bdJson{};
    if (const auto ec = glz::write_json(bd, bdJson); ec)
    {
        LOG_ERROR << "MissionStart: serialize battleData: " << glz::format_error(ec, bdJson);
        co_return HandleResult::success("{}");
    }
    // bdJson = {"pj41dy9g":[...],"75t0sx9z":[...],...}

    // ── Assemble final response ───────────────────────────────────────────────
    //
    // fEi17cnx is a 2-entry array:
    //   [0] = UserTeamInfo (player stats, from twJson)
    //   [1] = MissionStartInfo (reinforce friend + mission context)
    //
    // twJson = {"fEi17cnx":[{...}]}  — strip the trailing ]} to open the array
    // bdJson = {"pj41dy9g":[...],...} — strip { and } to merge into root

    if (twJson.size() < 2 || twJson.back() != '}' || twJson[twJson.size()-2] != ']')
    {
        LOG_ERROR << "MissionStart: unexpected twJson shape: " << twJson;
        co_return HandleResult::success("{}");
    }
    if (bdJson.empty() || bdJson.front() != '{' || bdJson.back() != '}')
    {
        LOG_ERROR << "MissionStart: unexpected bdJson shape";
        co_return HandleResult::success("{}");
    }

    std::string resp;
    resp.reserve(twJson.size() + bdJson.size() + 4096);

    // Open fEi17cnx array with UserTeamInfo as [0], then append MissionStartInfo as [1].
    // Z0Y4RoD7 in [1] tells the battle engine which squad deck to load — use the
    // deck the player selected when tapping "Start", not a hardcoded index.
    resp = twJson.substr(0, twJson.size() - 2);
    resp += R"(,{"h7eY3sAK":"n9ZMPC0t","J3stQ7jd":"42640","j28VNcUW":")";
    resp += missionIdStr;
    resp += R"(","Z0Y4RoD7":")";
    resp += std::to_string(activeDeck);
    resp += R"("}])";

    // Status key (battle result placeholder)
    resp += R"(,"6FrKacq7":[{"Kn51uR4Y":"0h6Q08SL"}])";

    // Reinforce/friend user snapshot (captured live-server data for user n9ZMPC0t)
    resp += R"(,"zI2tJB7R":[{"h7eY3sAK":"n9ZMPC0t","dD64grYH":"334","3w6YDS4z":"20","d96tuT2E":"36216751","pThS5FE3":"152762744","mn5Tj3fz":"125539575","jG91JRxN":"36137127","06phPeqv":"35791092","Zq8ej5IN":"346035","isRx41jy":"147217074","20qd9shE":"48090082","Sf95jez7":146280,"I29Qgxot":"124800","WMC6rNF1":"702","Z93pUQhG":"2180","Rc6St9h1":"45","k5Sjn9Zq":"7849","c3Bo97kI":"2674","Gt2msFb1":"7050","07HgoLtC":"1","AEz43gai":"0","8CEu9Kcm":"0","3DBVLY8H":"2887","c4im6B2v":2092,"UCN04WxE":"1994","ovFJ6Hp0":"15045","TW1Mrtp5":"502","5NRJQ1LU":"824760049","XP06YWdT":"569","U8uZLA34":"666505","rZQJF5G9":"51673","0LwvAF3H":"10523","rQ3TAy6I":"10116","hoG2ieT5":"5461379","6PLsn8xo":"2114221","84BC2kXw":"3418","5pg7MYCQ":"576","mFID53JZ":"1124","e6BKoYy9":"1814061344"}])";

    // tojMy68W — FriendInfoResponse.  Populates the FriendInfoList
    // singleton the squad-select reinforce picker reads from.  Replaces
    // the earlier xZH6EIQ7 + T_FIXED_REINFORCEMENT emission, which fed
    // ReinforcementInfoList / FixedReinforcementInfoList — both correctly
    // received by the binary but neither consulted by the squad-select UI
    // (confirmed via tools/ida/audits/tojMy68W_audit.txt — FriendInfoList
    // is the singleton populated at 0x13D93C0).  zI2tJB7R above still
    // carries the friend's account-stats archive blob for the per-friend
    // detail / online-status display.
    if (haveFriend)
    {
        std::string friendArrJson;
        if (const auto ec = glz::write_json(
                std::vector<FriendInfo>{friend_entry}, friendArrJson); ec)
        {
            LOG_WARN << "MissionStart: serialize tojMy68W: "
                     << glz::format_error(ec, friendArrJson);
        }
        else
        {
            resp += R"(,"tojMy68W":)";
            resp += friendArrJson;
        }
    }

    // Bonus info
    resp += R"(,"Kz7qfSs5":[{"k9cxD7Ba":"58844709","j3g5P4cq":"1","nA95Bdj6":"0","5Z1LNoyH":"0","LE6JkUp7":"1|0:25:30030:1:1| @2|0:25:10030:1:1| @3| |1/1/4:25:10000:1@4|1:30:50030:2:2| @5| | "}])";

    // Battle arrays — splice in by stripping enclosing { }
    resp += ',';
    resp.append(bdJson.begin() + 1, bdJson.end() - 1);

    // Misc empty/status arrays
    resp += R"(,"8hoyIF9Q":[],"VZwB7f3j":[],"nAligJSQ":[{"Kn51uR4Y":"0"}])";

    // Client session identity (IKqx1Cn9)
    resp += ",\"IKqx1Cn9\":[{\"h7eY3sAK\":\"";
    resp += std::string(kUserId);
    resp += "\",\"B5JQyV8j\":\"";
    resp += username;
    resp += R"(","iN7buP0j":"WAS-LX1A_android8.0.0","Ma5GnU0H":"4e457983-74b0-4ea7-9a98-1c5890dfc836"}])";

    // Announcement (closes root object)
    resp += R"(,"Pj6zDW3m":{"xJNom6i0":"3876","jsRoN50z":"http://ios21900.bfww.gumi.sg//news.gumi.sg/bravefrontier/news/files/html/2022-03/Closure_Announcement_033022_1648608188.html"}})";

    co_return HandleResult::success(resp);
}
