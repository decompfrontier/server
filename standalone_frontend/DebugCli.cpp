#include "DebugCli.hpp"

// ============================================================================
// Windows-only implementation.  Non-Windows stubs follow at the bottom.
// ============================================================================
#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <gimuserver/App.hpp>
#include <gimuserver/drogon/GimuServer.hpp>
#include <gimuserver/utils/Macros.hpp>

#include <drogon/drogon.h>

#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// ASCII SOH (0x01) used as the end-of-response sentinel. Never appears in
// normal console output, so the client can split on it unambiguously.
static constexpr char kEot = '\x01';

// The offline server hosts exactly one user, created dynamically by the tutorial
// (CreateUser assigns a random user_info.id). Migrations no longer seed a fixed
// dev user, so the id is resolved from the DB at runtime — see RequireUserId.
// Under the merged schema user_units.user_id == user_info.id (addUserUnit writes
// identity.userId to both), so one id serves every query in this file.

// ---------------------------------------------------------------------------
// Helpers shared by command handlers
// ---------------------------------------------------------------------------

namespace
{

struct CapacityInfo { int64_t cap, cur, free; };

// Resolve the single offline user's id, or print guidance and return empty when
// the tutorial hasn't created a user yet.
std::string RequireUserId(const drogon::orm::DbClientPtr& db)
{
    auto r = db->execSqlSync("SELECT id FROM user_info LIMIT 1;");
    if (r.empty())
    {
        std::cout << "  No user exists yet. Complete the tutorial (CreateUser) in "
                     "the client first, then retry.\n";
        return {};
    }
    return r[0][0].as<std::string>();
}

// The server grants a built-in 100 unit slots through UserTeamInfo.add_unit_count
// (hardcoded in gme::getTeamInfo, Common.hpp); the DB's max_unit_count is a
// separate expandable base that is never seeded and stays 0. Effective capacity
// the client enforces is max_unit_count + add_unit_count, so mirror that here —
// otherwise the CLI would refuse to add units the client actually has room for,
// or (if we raised max_unit_count blindly) let us overfill and desync the client.
static constexpr int64_t kAddUnitCount = 100;

CapacityInfo QueryCapacity(const drogon::orm::DbClientPtr& db, const std::string& userId)
{
    CapacityInfo c{};
    auto cr = db->execSqlSync("SELECT max_unit_count FROM user_info WHERE id=$1;",
                              userId);
    const int64_t maxUnitCount = cr.size() > 0 ? cr[0][0].as<int64_t>() : 0;
    c.cap = maxUnitCount + kAddUnitCount;

    auto nr = db->execSqlSync("SELECT COUNT(*) FROM user_units WHERE user_id=$1;",
                              userId);
    if (nr.size() > 0) c.cur = nr[0][0].as<int64_t>();

    c.free = std::max<int64_t>(0, c.cap - c.cur);
    return c;
}

bool ParseInt(const std::string& s, int64_t& out)
{
    try { out = std::stoll(s); return true; }
    catch (...) { return false; }
}

// Client overflows zel/karma above 99 999 999 and resets to 0, which breaks
// evo/mix cost checks.  Hard-cap every write so the DB never goes above this.
// (Seed values in MigrationManager remain at 99 000 000 to leave headroom.)
static constexpr int64_t kMaxZelKarma = 99'999'999LL;

// Returns the UnitMst element id (1–6) for a name, or 0 if unrecognised.
static int32_t ParseElement(const std::string& s)
{
    if (s == "fire")    return 1;
    if (s == "water")   return 2;
    if (s == "earth")   return 3;
    if (s == "thunder") return 4;
    if (s == "light")   return 5;
    if (s == "dark")    return 6;
    return 0;
}


// ---------------------------------------------------------------------------
// Command handlers — all write to std::cout; caller redirects rdbuf to capture
// ---------------------------------------------------------------------------

void PrintHelp()
{
    std::cout
        << "\n  Debug CLI commands:\n"
        << "    help                 Show this list.\n"
        << "    zel <n>              Set zel balance (capped at 99 999 999).\n"
        << "    karma <n>            Set karma balance (capped at 99 999 999).\n"
        << "    gems <n>             Set gem balance (used for summoning).\n"
        << "    addunit <mst_id>     Add one unit by MST id (capacity-checked).\n"
        << "    addunit all          Add one of every unit in the MST, up to capacity.\n"
        << "    addunit mat          Fill remaining capacity with evo-material units, even-spread.\n"
        << "    addunit enh          Fill remaining capacity with enhancer/XP-feeder units, even-spread.\n"
        << "    addunit <element>    Fill remaining capacity with mat+enh mix for one element.\n"
        << "                         Elements: fire water earth thunder light dark\n"
        << "    clearunits           Delete all units except the oldest (slot 1).\n"
        << "    cap                  Show current / max unit count.\n"
        << "    additem <mst_id> [n] Add n of an item (default 1, clamped to MST max stack).\n"
        << "    additem usable [n]   Add every usable/battle item (potions etc.), n each (default 10).\n"
        << "    additem sphere [n]   Add every sphere (equipment), n each (default 1).\n"
        << "    additem mat [n]      Add every crafting material, n each (default 50).\n"
        << "    additem evo [n]      Add every evolution item, n each (default 1).\n"
        << "    additem all [n]      Add everything in the item MST, n each (default 1).\n"
        << "    items                List owned item stacks (id, name, count).\n"
        << "    clearitems           Delete all item stacks and unequip spheres from units.\n"
        << "    unlockmap            Unlock every story mission on the campaign map.\n"
        << "    resetmap             Clear all campaign map progression.\n"
        << "    unlocktown           Unlock the town: provision facilities/locations,\n"
        << "                         finish the tutorial, mark missions 1+2 cleared.\n"
        << "    clearmission <id>    Mark one mission cleared (feeds the UT1SVg59 unlock list).\n"
        << "    resetscenarios       Forget viewed cutscenes (they replay on next trigger).\n"
        << "    quit                 Shut down the server and close this window.\n\n";
}

void DoSetIntColumn(const drogon::orm::DbClientPtr& db, const std::string& userId,
                    const std::string& column, int64_t value, const std::string& label,
                    int64_t maxVal = std::numeric_limits<int64_t>::max())
{
    if (value > maxVal)
    {
        std::cout << "  Warning: " << value << " exceeds safe max (" << maxVal
                  << "). Clamping.\n";
        value = maxVal;
    }
    db->execSqlSync("UPDATE user_info SET " + column + "=$1 WHERE id=$2;",
                    value, userId);
    std::cout << "  " << label << " = " << value << "\n";
}

void DoAddUnit(const drogon::orm::DbClientPtr& db, const std::string& userId, int32_t mstId)
{
    const auto& mst = theServer()->cache().unitMst();
    auto it = std::find_if(mst.begin(), mst.end(),
        [mstId](const UnitMst& u) { return u.id == mstId; });
    if (it == mst.end()) { std::cout << "  No MST entry with id " << mstId << ".\n"; return; }

    auto c = QueryCapacity(db, userId);
    if (c.free <= 0) { std::cout << "  Inventory full (" << c.cur << "/" << c.cap << ").\n"; return; }

    try
    {
        GimuServer::InsertUnitFromMst(db, userId, *it);
        std::cout << "  Added unit " << mstId << " (" << it->unit_name << ").\n";
    }
    catch (const drogon::orm::DrogonDbException& e)
    {
        std::cout << "  INSERT failed: " << e.base().what() << "\n";
    }
}

void DoAddAll(const drogon::orm::DbClientPtr& db, const std::string& userId)
{
    // Build a set of already-owned base unit_ids (plain MST id strings, e.g.
    // "10011") so we add at most one copy of each distinct unit from the MST.
    // Evolved copies ("10011_100") are stored with a suffix and won't block
    // re-adding the base form.
    const auto ownedRes = db->execSqlSync(
        "SELECT DISTINCT unit_id FROM user_units WHERE user_id=$1;",
        userId);
    std::unordered_set<std::string> owned;
    owned.reserve(ownedRes.size());
    for (const auto& row : ownedRes)
        owned.insert(row[0].as<std::string>());

    const auto& mst = theServer()->cache().unitMst();
    auto c = QueryCapacity(db, userId);
    if (c.free <= 0) { std::cout << "  Inventory full (" << c.cur << "/" << c.cap << ").\n"; return; }

    int64_t added = 0, skipped = 0;
    for (const auto& unit : mst)
    {
        if (unit.id == 1) continue;
        if (added >= c.free) { ++skipped; continue; }
        const std::string uid = std::to_string(unit.id);
        if (owned.count(uid)) { ++skipped; continue; }
        try
        {
            GimuServer::InsertUnitFromMst(db, userId, unit);
            ++added;
        }
        catch (const drogon::orm::DrogonDbException& e)
        {
            std::cout << "  DB error: " << e.base().what() << "\n";
            ++skipped;
        }
    }
    std::cout << "  Added " << added << " units";
    if (skipped > 0) std::cout << ", skipped " << skipped << " (cap or already owned)";
    std::cout << ".\n";
}

void FillEvenSpread(const drogon::orm::DbClientPtr& db, const std::string& userId,
                    const std::function<bool(const UnitMst&)>& predicate,
                    const std::string& label)
{
    const auto& mst = theServer()->cache().unitMst();

    // Bucket candidates by (rarity, element) for an even round-robin fill.
    std::map<std::pair<int32_t, int32_t>, std::vector<const UnitMst*>> buckets;
    for (const auto& unit : mst)
    {
        if (unit.id == 1) continue;
        if (predicate(unit))
            buckets[{unit.rarity, unit.element}].push_back(&unit);
    }

    if (buckets.empty()) { std::cout << "  No " << label << " candidates in MST.\n"; return; }

    auto c = QueryCapacity(db, userId);
    if (c.free <= 0) { std::cout << "  Inventory full (" << c.cur << "/" << c.cap << ").\n"; return; }

    // Per-bucket cursor — wraps around so slots are filled even when pool is
    // smaller than free capacity.  UNIQUE(user_id, unit_id) has been dropped,
    // so duplicate MST entries are allowed and inserts always succeed.
    std::map<std::pair<int32_t, int32_t>, size_t> cursors;
    for (const auto& [k, _] : buckets) cursors[k] = 0;

    int64_t added = 0;
    bool dbError = false;
    while (added < c.free && !dbError)
    {
        for (auto& [key, vec] : buckets)
        {
            if (added >= c.free || dbError) break;
            if (vec.empty()) continue;

            const UnitMst* unit = vec[cursors[key] % vec.size()];
            ++cursors[key];

            try
            {
                GimuServer::InsertUnitFromMst(db, userId, *unit);
                ++added;
            }
            catch (const drogon::orm::DrogonDbException& e)
            {
                std::cout << "  DB error during fill: " << e.base().what() << "\n";
                dbError = true;
            }
        }
    }

    std::cout << "  Filled " << added << " " << label << " units across "
              << buckets.size() << " (rarity, element) buckets.\n";
}

void DoAddMat(const drogon::orm::DbClientPtr& db, const std::string& userId)
{
    // kind 2 = evo-material fodder (Pot / Bulb / Idol / Nymph lines).
    // kind 3 = special evo materials (5 light-element units); include both.
    FillEvenSpread(db, userId,
        [](const UnitMst& u) { return u.kind == 2 || u.kind == 3; },
        "material");
}

void DoAddEnh(const drogon::orm::DbClientPtr& db, const std::string& userId)
{
    // kind 1 = XP-feeder / enhancer units (Metal Ghost / King / God lines,
    // Omni Frog / Omni Emperor, etc.).
    FillEvenSpread(db, userId,
        [](const UnitMst& u) { return u.kind == 1; },
        "enhancer");
}

void DoAddElement(const drogon::orm::DbClientPtr& db, const std::string& userId, int32_t elementId)
{
    // Fill with mat (kind 2/3) AND enh (kind 1) units of the chosen element.
    FillEvenSpread(db, userId,
        [elementId](const UnitMst& u) {
            return u.element == elementId && (u.kind == 1 || u.kind == 2 || u.kind == 3);
        },
        "element");
}

void DoClearUnits(const drogon::orm::DbClientPtr& db, const std::string& userId)
{
    db->execSqlSync(
        "DELETE FROM user_units WHERE user_id=$1 "
        "AND user_unit_id <> (SELECT MIN(user_unit_id) FROM user_units WHERE user_id=$1);",
        userId);
    auto c = QueryCapacity(db, userId);
    std::cout << "  Cleared. Now at " << c.cur << "/" << c.cap << ".\n";
}

// Unlock the full story campaign map. The client only shows missions whose row
// exists in user_campaign_missions (see CampaignBattleEnd), so this inserts an
// "available" row (state=1) for every story mission. Story = mission MST lands
// 1-20; land 0 is a placeholder and land 99+ is event/vortex/raid content that
// lives outside the world-map progression (and would bloat the list by 2000+).
// INSERT OR IGNORE preserves any mission already cleared (state=2) by real play.
void DoUnlockMap(const drogon::orm::DbClientPtr& db, const std::string& userId)
{
    const auto& missions = theServer()->cache().missionMst();

    // One multi-row INSERT: userId is bound once as $1 (referenced by every
    // tuple — a supported pattern), mission ids are integers from the trusted
    // MST so they inline safely.
    std::string values;
    int64_t n = 0;
    for (const auto& m : missions)
    {
        if (m.land_id < 1 || m.land_id > 20) continue;
        if (!values.empty()) values += ",";
        values += "($1,'" + std::to_string(m.id) + "',1,0)";
        ++n;
    }

    if (n == 0) { std::cout << "  No story missions found in mission MST.\n"; return; }

    db->execSqlSync(
        "INSERT OR IGNORE INTO user_campaign_missions"
        " (user_id, mission_id, state, attain_percent) VALUES " + values + ";",
        userId);
    std::cout << "  Unlocked " << n << " story missions on the campaign map.\n";
}

void DoResetMap(const drogon::orm::DbClientPtr& db, const std::string& userId)
{
    db->execSqlSync("DELETE FROM user_campaign_missions WHERE user_id=$1;", userId);
    // Also clear the active-mission pointer so the next campaign run starts
    // clean rather than resuming a mission whose row we just deleted.
    try
    {
        db->execSqlSync("DELETE FROM user_campaign_state WHERE user_id=$1;", userId);
    }
    catch (const drogon::orm::DrogonDbException& ex)
    {
        LOG_DEBUG << "resetmap: user_campaign_state clear skipped: " << ex.base().what();
    }
    std::cout << "  Reset campaign map progression.\n";
}

// ---------------------------------------------------------------------------
// Item commands — operate on user_items (stacked by user_id + item_id).
// ---------------------------------------------------------------------------

// ItemMst.item_type labels (decoded 2026-07-17 from the data + legacy MST;
// see packet-generator/assets/mst/item.kdl).
const char* ItemTypeLabel(int32_t type)
{
    switch (type)
    {
    case 1:  return "usable";
    case 2:  return "material";
    case 3:  return "sphere";
    case 4:  return "evo item";
    case 5:  return "token";
    case 6:  return "sphere+";
    default: return "?";
    }
}

// Truncate a UTF-8 string to at most maxBytes without splitting a codepoint
// (JP item names are multi-byte; a mid-sequence cut garbles the console).
std::string Utf8Truncate(const std::string& s, size_t maxBytes)
{
    if (s.size() <= maxBytes) return s;
    size_t cut = maxBytes;
    while (cut > 0 && (static_cast<unsigned char>(s[cut]) & 0xC0) == 0x80)
        --cut;
    return s.substr(0, cut) + "..";
}

// UPSERT one stack, clamped to the MST max stack.  Returns the resulting count.
int64_t UpsertItemStack(const drogon::orm::DbClientPtr& db, const std::string& userId,
                        int32_t itemId, int64_t add, int64_t maxStack)
{
    if (maxStack <= 0) maxStack = 1;
    db->execSqlSync(
        "INSERT INTO user_items (user_id, item_id, item_num) VALUES ($1, $2, MIN($3, $4)) "
        "ON CONFLICT(user_id, item_id) DO UPDATE SET item_num = MIN($4, item_num + $3);",
        userId, itemId, add, maxStack);
    auto r = db->execSqlSync(
        "SELECT item_num FROM user_items WHERE user_id=$1 AND item_id=$2;", userId, itemId);
    return r.empty() ? 0 : r[0][0].as<int64_t>();
}

void DoAddItem(const drogon::orm::DbClientPtr& db, const std::string& userId,
               int32_t itemId, int64_t count)
{
    const auto& mst = theServer()->cache().itemMst();
    auto it = std::find_if(mst.begin(), mst.end(),
        [itemId](const ItemMst& m) { return m.id == itemId; });
    if (it == mst.end()) { std::cout << "  No item MST entry with id " << itemId << ".\n"; return; }

    const int64_t now = UpsertItemStack(db, userId, itemId, count, it->max_stack);
    std::cout << "  " << Utf8Truncate(it->name, 40) << " (" << itemId << ", "
              << ItemTypeLabel(it->item_type) << ") x" << now
              << (now >= it->max_stack ? " [stack full]" : "") << "\n";
}

// Bulk fill: one stack of every MST entry matching the predicate, n each.
void DoAddItemBulk(const drogon::orm::DbClientPtr& db, const std::string& userId,
                   const std::function<bool(const ItemMst&)>& predicate,
                   int64_t count, const std::string& label)
{
    const auto& mst = theServer()->cache().itemMst();
    int64_t species = 0;
    for (const auto& m : mst)
    {
        if (!predicate(m)) continue;
        UpsertItemStack(db, userId, m.id, count, m.max_stack);
        ++species;
    }
    if (species == 0) { std::cout << "  No " << label << " entries in the item MST.\n"; return; }
    std::cout << "  Stacked " << species << " " << label << " species (x" << count
              << " each, clamped per max stack).\n";
}

void DoListItems(const drogon::orm::DbClientPtr& db, const std::string& userId)
{
    // Name/type lookup by id, built once per call (1668 entries — cheap).
    const auto& mst = theServer()->cache().itemMst();
    std::unordered_map<int32_t, const ItemMst*> byId;
    byId.reserve(mst.size());
    for (const auto& m : mst) byId[m.id] = &m;

    auto rows = db->execSqlSync(
        "SELECT instance_id, item_id, item_num, favorite_flg FROM user_items "
        "WHERE user_id=$1 AND item_num > 0 ORDER BY item_id;", userId);
    if (rows.empty()) { std::cout << "  No items owned. Try 'additem usable'.\n"; return; }

    int64_t total = 0;
    for (const auto& row : rows)
    {
        const int32_t itemId = row["item_id"].as<int32_t>();
        const int64_t num    = row["item_num"].as<int64_t>();
        const auto* m = byId.count(itemId) ? byId[itemId] : nullptr;
        total += num;
        std::cout << "  " << std::setw(9) << itemId << "  x" << std::setw(4) << std::left
                  << num << std::right
                  << (row["favorite_flg"].as<int32_t>() != 0 ? " * " : "   ")
                  << std::setw(8) << (m ? ItemTypeLabel(m->item_type) : "?") << "  "
                  << Utf8Truncate(m ? m->name : "(not in MST)", 40) << "\n";
    }
    std::cout << "  " << rows.size() << " stacks, " << total << " items total. (* = favorited)\n";
}

void DoClearItems(const drogon::orm::DbClientPtr& db, const std::string& userId)
{
    db->execSqlSync("DELETE FROM user_items WHERE user_id=$1;", userId);
    // Also clear sphere slots so units don't reference items that no longer
    // exist in the warehouse.
    db->execSqlSync(
        "UPDATE user_units SET eqip_item_id=0, eqip_item_frame_id=0, "
        "eqip_item_id2=0, eqip_item_frame_id2=0 WHERE user_id=$1;", userId);
    std::cout << "  Cleared all item stacks and unequipped spheres from units.\n";
}

// ---------------------------------------------------------------------------
// Progression commands — town unlock, mission clears, cutscene state.
// ---------------------------------------------------------------------------

// Mark one mission cleared in user_campaign_missions (state=2).  UserInfo
// reports the cleared set as UT1SVg59 (UserClearMissionInfo), which the client
// evaluates feature unlocks against (F_FUNCTION_RELEASE_MST type-2 conditions
// + the hardcoded town/early-feature gates).
void MarkMissionCleared(const drogon::orm::DbClientPtr& db, const std::string& userId,
                        int64_t missionId)
{
    const auto now = static_cast<int64_t>(std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()).count());
    db->execSqlSync(
        "INSERT INTO user_campaign_missions"
        " (user_id, mission_id, state, attain_percent, clear_count, last_cleared_at)"
        " VALUES ($1, $2, 2, 100, 1, $3)"
        " ON CONFLICT(user_id, mission_id) DO UPDATE SET"
        " state=2, attain_percent=100,"
        " clear_count=MAX(1, clear_count), last_cleared_at=$3;",
        userId, std::to_string(missionId), now);
}

void DoClearMission(const drogon::orm::DbClientPtr& db, const std::string& userId,
                    int64_t missionId)
{
    MarkMissionCleared(db, userId, missionId);
    std::cout << "  Mission " << missionId << " marked cleared (UT1SVg59 list).\n";
}

// Unlock the town for testing.  Legit unlock is progression-driven (tutorial
// completion + early mission clears evaluated client-side); this provisions
// the same state directly:
//   1. tutorial_status -> 12 (tutorial complete; never downgrades),
//   2. missions 1+2 marked cleared (the tutorial missions),
//   3. one lv-1 row per non-event town facility (MST id < 1000 — ids >= 1000
//      are Event Bazaar entries with no bundled sprites; seeding them crashes
//      the town scene, see handbook §3.3) and per town location.
// UserInfo then reports non-empty town arrays and the client renders the town.
void DoUnlockTown(const drogon::orm::DbClientPtr& db, const std::string& userId)
{
    db->execSqlSync(
        "UPDATE user_info SET tutorial_status=MAX(tutorial_status, 12) WHERE id=$1;",
        userId);
    MarkMissionCleared(db, userId, 1);
    MarkMissionCleared(db, userId, 2);

    const auto& init = theServer()->cache().initializeResp();
    int64_t facilities = 0, locations = 0;
    for (const auto& f : init.town_facility)
    {
        if (f.id >= 1000) continue;
        db->execSqlSync(
            "INSERT OR IGNORE INTO user_town_facilities (user_id, facility_id, lv)"
            " VALUES ($1, $2, 1);",
            userId, f.id);
        ++facilities;
    }
    for (const auto& l : init.town_location)
    {
        db->execSqlSync(
            "INSERT OR IGNORE INTO user_town_locations (user_id, location_id, lv)"
            " VALUES ($1, $2, 1);",
            userId, l.id);
        ++locations;
    }

    std::cout << "  Town unlocked: tutorial complete, missions 1+2 cleared, "
              << facilities << " facilities + " << locations
              << " locations provisioned at lv 1.\n"
              << "  Restart the client (or reload UserInfo) to see it.\n";
}

void DoResetScenarios(const drogon::orm::DbClientPtr& db, const std::string& userId)
{
    db->execSqlSync("DELETE FROM user_scenarios WHERE user_id=$1;", userId);
    std::cout << "  Viewed-cutscene state cleared — cutscenes replay on next trigger.\n";
}

// ---------------------------------------------------------------------------
// Command dispatcher
// Returns true always; sets wantsQuit if the server should shut down.
// ---------------------------------------------------------------------------

void DispatchCommand(const std::string& line, bool& wantsQuit)
{
    wantsQuit = false;
    if (line.empty()) return;

    std::istringstream iss(line);
    std::string cmd;
    if (!(iss >> cmd)) return;

    auto db = drogon::app().getDbClient();

    if (cmd == "help")
    {
        PrintHelp();
    }
    else if (cmd == "quit" || cmd == "exit")
    {
        std::cout << "  Shutting down server...\n";
        drogon::app().getLoop()->queueInLoop([]() { drogon::app().quit(); });
        wantsQuit = true;
    }
    else if (cmd == "cap")
    {
        const auto userId = RequireUserId(db);
        if (userId.empty()) return;
        auto c = QueryCapacity(db, userId);
        std::cout << "  " << c.cur << "/" << c.cap << " (" << c.free << " free)\n";
    }
    else if (cmd == "zel" || cmd == "karma" || cmd == "gems")
    {
        std::string arg; int64_t v = 0;
        if (!(iss >> arg) || !ParseInt(arg, v))
            { std::cout << "  Usage: " << cmd << " <n>\n"; return; }
        const auto userId = RequireUserId(db);
        if (userId.empty()) return;
        // DB has a single `gems` column (client's paid-gem bucket; free_gems is
        // always 0 — see CampaignBattleEnd's "0 AS free_gems, gems AS paid_gems").
        const std::string col = (cmd == "gems") ? "gems" : cmd;
        const int64_t cap = (cmd == "zel" || cmd == "karma") ? kMaxZelKarma
                                                              : std::numeric_limits<int64_t>::max();
        DoSetIntColumn(db, userId, col, v, cmd, cap);
    }
    else if (cmd == "addunit")
    {
        std::string sub;
        if (!(iss >> sub)) { std::cout << "  Usage: addunit <mst_id|all|mat|enh|fire|water|earth|thunder|light|dark>\n"; return; }
        const auto userId = RequireUserId(db);
        if (userId.empty()) return;
        if      (sub == "all") DoAddAll(db, userId);
        else if (sub == "mat") DoAddMat(db, userId);
        else if (sub == "enh") DoAddEnh(db, userId);
        else if (int32_t elem = ParseElement(sub); elem != 0) DoAddElement(db, userId, elem);
        else
        {
            int64_t id = 0;
            if (!ParseInt(sub, id))
                { std::cout << "  Unknown subcommand '" << sub << "'.\n"; return; }
            DoAddUnit(db, userId, static_cast<int32_t>(id));
        }
    }
    else if (cmd == "clearunits")
    {
        const auto userId = RequireUserId(db);
        if (userId.empty()) return;
        DoClearUnits(db, userId);
    }
    else if (cmd == "additem")
    {
        std::string sub;
        if (!(iss >> sub))
            { std::cout << "  Usage: additem <mst_id|usable|sphere|mat|evo|all> [count]\n"; return; }
        const auto userId = RequireUserId(db);
        if (userId.empty()) return;

        // Optional per-species count; each bulk group has its own default.
        int64_t n = -1;
        {
            std::string arg;
            if (iss >> arg && !ParseInt(arg, n)) { std::cout << "  Bad count '" << arg << "'.\n"; return; }
        }
        if (n == 0 || n < -1) { std::cout << "  Count must be positive.\n"; return; }

        if (sub == "usable")
            DoAddItemBulk(db, userId,
                [](const ItemMst& m) { return m.item_type == 1 || m.item_type == 5; },
                n > 0 ? n : 10, "usable");
        else if (sub == "sphere")
            DoAddItemBulk(db, userId,
                [](const ItemMst& m) { return m.item_type == 3 || m.item_type == 6; },
                n > 0 ? n : 1, "sphere");
        else if (sub == "mat")
            DoAddItemBulk(db, userId,
                [](const ItemMst& m) { return m.item_type == 2; },
                n > 0 ? n : 50, "material");
        else if (sub == "evo")
            DoAddItemBulk(db, userId,
                [](const ItemMst& m) { return m.item_type == 4; },
                n > 0 ? n : 1, "evo-item");
        else if (sub == "all")
            DoAddItemBulk(db, userId,
                [](const ItemMst&) { return true; },
                n > 0 ? n : 1, "item");
        else
        {
            int64_t id = 0;
            if (!ParseInt(sub, id))
                { std::cout << "  Unknown subcommand '" << sub << "'.\n"; return; }
            DoAddItem(db, userId, static_cast<int32_t>(id), n > 0 ? n : 1);
        }
    }
    else if (cmd == "items")
    {
        const auto userId = RequireUserId(db);
        if (userId.empty()) return;
        DoListItems(db, userId);
    }
    else if (cmd == "clearitems")
    {
        const auto userId = RequireUserId(db);
        if (userId.empty()) return;
        DoClearItems(db, userId);
    }
    else if (cmd == "unlockmap")
    {
        const auto userId = RequireUserId(db);
        if (userId.empty()) return;
        DoUnlockMap(db, userId);
    }
    else if (cmd == "resetmap")
    {
        const auto userId = RequireUserId(db);
        if (userId.empty()) return;
        DoResetMap(db, userId);
    }
    else if (cmd == "unlocktown")
    {
        const auto userId = RequireUserId(db);
        if (userId.empty()) return;
        DoUnlockTown(db, userId);
    }
    else if (cmd == "clearmission")
    {
        std::string arg; int64_t id = 0;
        if (!(iss >> arg) || !ParseInt(arg, id) || id <= 0)
            { std::cout << "  Usage: clearmission <mission_id>\n"; return; }
        const auto userId = RequireUserId(db);
        if (userId.empty()) return;
        DoClearMission(db, userId, id);
    }
    else if (cmd == "resetscenarios")
    {
        const auto userId = RequireUserId(db);
        if (userId.empty()) return;
        DoResetScenarios(db, userId);
    }
    else
    {
        std::cout << "  Unknown command '" << cmd << "'. Type 'help'.\n";
    }
}

// Run a command and capture all std::cout output as a string.
// Drogon's logger does not write through std::cout, so redirecting rdbuf
// only captures our own output and never touches the server log window.
std::string RunCommand(const std::string& line, bool& wantsQuit)
{
    std::ostringstream capture;
    std::streambuf* old = std::cout.rdbuf(capture.rdbuf());
    try
    {
        DispatchCommand(line, wantsQuit);
    }
    catch (const drogon::orm::DrogonDbException& e)
    {
        std::cout << "  DB error: " << e.base().what() << "\n";
    }
    catch (const std::exception& e)
    {
        std::cout << "  Error: " << e.what() << "\n";
    }
    std::cout.rdbuf(old);
    return capture.str();
}

// ---------------------------------------------------------------------------
// Named-pipe write helper
// ---------------------------------------------------------------------------

static bool PipeWriteAll(HANDLE h, const char* data, DWORD len)
{
    for (DWORD done = 0; done < len; )
    {
        DWORD w = 0;
        if (!WriteFile(h, data + done, len - done, &w, NULL) || w == 0)
            return false;
        done += w;
    }
    return true;
}

static bool PipeSendResponse(HANDLE pipe, const std::string& output)
{
    // Payload = <text> + kEot sentinel byte.
    std::string payload = output;
    payload += kEot;
    return PipeWriteAll(pipe, payload.c_str(), static_cast<DWORD>(payload.size()));
}

// ---------------------------------------------------------------------------
// Pipe server loop — runs in a detached thread in the server process
// ---------------------------------------------------------------------------

static void PipeServerLoop(HANDLE pipe)
{
    if (!ConnectNamedPipe(pipe, NULL) && GetLastError() != ERROR_PIPE_CONNECTED)
    {
        LOG_ERROR << "DebugCli: ConnectNamedPipe failed (" << GetLastError() << ")";
        CloseHandle(pipe);
        return;
    }

    LOG_INFO << "DebugCli: CLI window connected.";

    // Send the help text automatically when the client first connects.
    {
        bool dummy = false;
        PipeSendResponse(pipe, RunCommand("help", dummy));
    }

    std::string partial;
    char buf[4096];

    while (true)
    {
        DWORD bytesRead = 0;
        if (!ReadFile(pipe, buf, sizeof(buf) - 1, &bytesRead, NULL) || bytesRead == 0)
            break;
        buf[bytesRead] = '\0';
        partial.append(buf, bytesRead);

        size_t pos;
        while ((pos = partial.find('\n')) != std::string::npos)
        {
            std::string cmd = partial.substr(0, pos);
            if (!cmd.empty() && cmd.back() == '\r') cmd.pop_back();
            partial = partial.substr(pos + 1);

            bool wantsQuit = false;
            const std::string output = RunCommand(cmd, wantsQuit);

            if (!PipeSendResponse(pipe, output)) goto done;
            if (wantsQuit) goto done;
        }
    }

done:
    CloseHandle(pipe);
    LOG_INFO << "DebugCli: pipe server exiting.";
}

} // anonymous namespace

// ---------------------------------------------------------------------------
// Public API — server side
// ---------------------------------------------------------------------------

void StartDebugCli()
{
    // Headless guard: when the server runs without a console (redirected
    // stdio, CI, test harness), CreateProcess(CREATE_NEW_CONSOLE) can wedge
    // in the console driver with an unkillable process. Only spawn the CLI
    // when the server itself owns a console window.
    if (GetConsoleWindow() == nullptr)
    {
        LOG_INFO << "StartDebugCli: no console attached; skipping debug CLI";
        return;
    }

    const std::string pipeName =
        R"(\\.\pipe\gimudebug_)" + std::to_string(GetCurrentProcessId());

    // Create the pipe BEFORE spawning the child so it exists when the child
    // process calls WaitNamedPipe / CreateFile during startup.
    HANDLE pipe = CreateNamedPipeA(
        pipeName.c_str(),
        PIPE_ACCESS_DUPLEX,
        PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
        1,      // max instances
        65536,  // out-buffer
        65536,  // in-buffer
        0,
        NULL
    );
    if (pipe == INVALID_HANDLE_VALUE)
    {
        LOG_ERROR << "StartDebugCli: CreateNamedPipe failed (" << GetLastError() << ")";
        return;
    }

    // Locate this executable so we can re-spawn it in CLI-client mode.
    char exePath[MAX_PATH] = {};
    GetModuleFileNameA(NULL, exePath, MAX_PATH);

    std::string cmdLine = "\"" + std::string(exePath) + "\" --debug-cli " + pipeName;

    STARTUPINFOA si{};
    si.cb = sizeof(si);
    PROCESS_INFORMATION pi{};

    if (!CreateProcessA(NULL, cmdLine.data(), NULL, NULL, FALSE,
                        CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi))
    {
        LOG_ERROR << "StartDebugCli: CreateProcess failed (" << GetLastError() << ")";
        CloseHandle(pipe);
        return;
    }
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);

    std::thread([pipe]() { PipeServerLoop(pipe); }).detach();
}

// ---------------------------------------------------------------------------
// Public API — CLI-client side (runs in the spawned console window process)
// ---------------------------------------------------------------------------

void RunDebugCliClient(const char* pipeName)
{
    SetConsoleTitleW(L"GimuFrontier debug CLI");
    // Item/unit names from the MST are UTF-8 (JP text); the default OEM
    // codepage garbles them.
    SetConsoleOutputCP(CP_UTF8);

    // Retry connecting; the server creates the pipe before spawning us, but
    // process startup adds a small delay.
    HANDLE pipe = INVALID_HANDLE_VALUE;
    for (int i = 0; i < 40 && pipe == INVALID_HANDLE_VALUE; ++i)
    {
        WaitNamedPipeA(pipeName, 5000);
        pipe = CreateFileA(pipeName,
                           GENERIC_READ | GENERIC_WRITE,
                           0, NULL, OPEN_EXISTING, 0, NULL);
        if (pipe == INVALID_HANDLE_VALUE) Sleep(250);
    }

    if (pipe == INVALID_HANDLE_VALUE)
    {
        std::cerr << "[debug-cli] Could not connect to server pipe ("
                  << GetLastError() << ").\nPress Enter to close.\n";
        std::cin.get();
        return;
    }

    // Read bytes from the pipe until the kEot sentinel, then print to stdout.
    auto readResponse = [&]() -> bool {
        std::string buf;
        char ch;
        DWORD rd;
        while (true)
        {
            if (!ReadFile(pipe, &ch, 1, &rd, NULL) || rd == 0) return false;
            if (ch == kEot) break;
            buf += ch;
        }
        if (!buf.empty()) std::cout << buf;
        return true;
    };

    // Server sends the help text automatically on connect.
    if (!readResponse()) return;

    std::string line;
    while (true)
    {
        std::cout << "dev> " << std::flush;
        if (!std::getline(std::cin, line)) break;
        if (!line.empty() && line.back() == '\r') line.pop_back();

        const std::string toSend = line + "\n";
        DWORD written = 0;
        if (!WriteFile(pipe, toSend.c_str(), static_cast<DWORD>(toSend.size()), &written, NULL))
            break;

        if (!readResponse()) break;
        if (line == "quit" || line == "exit") break;
    }

    CloseHandle(pipe);
    std::cout << "\nPress Enter to close.\n";
    std::cin.get();
}

#else // _WIN32

// ---------------------------------------------------------------------------
// Non-Windows stubs
// ---------------------------------------------------------------------------

void StartDebugCli() {}
void RunDebugCliClient(const char*) {}

#endif // _WIN32
