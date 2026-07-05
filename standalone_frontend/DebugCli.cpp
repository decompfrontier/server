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
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <thread>
#include <unordered_set>
#include <vector>

// ASCII SOH (0x01) used as the end-of-response sentinel. Never appears in
// normal console output, so the client can split on it unambiguously.
static constexpr char kEot = '\x01';

// Hard-coded dev-user identifiers (single offline user — see MigrationManager).
static constexpr const char* kInfoId       = "12345678";
static constexpr const char* kPacketUserId = "0839899613932562";

// ---------------------------------------------------------------------------
// Helpers shared by command handlers
// ---------------------------------------------------------------------------

namespace
{

struct CapacityInfo { int64_t cap, cur, free; };

CapacityInfo QueryCapacity(const drogon::orm::DbClientPtr& db)
{
    CapacityInfo c{};
    auto cr = db->execSqlSync("SELECT max_unit_count FROM user_info WHERE id=$1;",
                              std::string(kInfoId));
    if (cr.size() > 0) c.cap = cr[0][0].as<int64_t>();

    auto nr = db->execSqlSync("SELECT COUNT(*) FROM user_units WHERE user_id=$1;",
                              std::string(kPacketUserId));
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
        << "    zel <n>              Set zel balance (capped at 99 000 000).\n"
        << "    karma <n>            Set karma balance (capped at 99 000 000).\n"
        << "    gems <n>             Set free gem balance.\n"
        << "    addunit <mst_id>     Add one unit by MST id (capacity-checked).\n"
        << "    addunit all          Add one of every unit in the MST, up to capacity.\n"
        << "    addunit mat          Fill remaining capacity with evo-material units, even-spread.\n"
        << "    addunit enh          Fill remaining capacity with enhancer/XP-feeder units, even-spread.\n"
        << "    addunit <element>    Fill remaining capacity with mat+enh mix for one element.\n"
        << "                         Elements: fire water earth thunder light dark\n"
        << "    clearunits           Delete all units except the oldest (slot 1).\n"
        << "    cap                  Show current / max unit count.\n"
        << "    quit                 Shut down the server and close this window.\n\n";
}

void DoSetIntColumn(const drogon::orm::DbClientPtr& db,
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
                    value, std::string(kInfoId));
    std::cout << "  " << label << " = " << value << "\n";
}

void DoAddUnit(const drogon::orm::DbClientPtr& db, int32_t mstId)
{
    const auto& mst = theServer()->cache().unitMst();
    auto it = std::find_if(mst.begin(), mst.end(),
        [mstId](const UnitMst& u) { return u.id == mstId; });
    if (it == mst.end()) { std::cout << "  No MST entry with id " << mstId << ".\n"; return; }

    auto c = QueryCapacity(db);
    if (c.free <= 0) { std::cout << "  Inventory full (" << c.cur << "/" << c.cap << ").\n"; return; }

    try
    {
        GimuServer::InsertUnitFromMst(db, kPacketUserId, *it);
        std::cout << "  Added unit " << mstId << " (" << it->unit_name << ").\n";
    }
    catch (const drogon::orm::DrogonDbException& e)
    {
        std::cout << "  INSERT failed: " << e.base().what() << "\n";
    }
}

void DoAddAll(const drogon::orm::DbClientPtr& db)
{
    // Build a set of already-owned base unit_ids (plain MST id strings, e.g.
    // "10011") so we add at most one copy of each distinct unit from the MST.
    // Evolved copies ("10011_100") are stored with a suffix and won't block
    // re-adding the base form.
    const auto ownedRes = db->execSqlSync(
        "SELECT DISTINCT unit_id FROM user_units WHERE user_id=$1;",
        std::string(kPacketUserId));
    std::unordered_set<std::string> owned;
    owned.reserve(ownedRes.size());
    for (const auto& row : ownedRes)
        owned.insert(row[0].as<std::string>());

    const auto& mst = theServer()->cache().unitMst();
    auto c = QueryCapacity(db);
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
            GimuServer::InsertUnitFromMst(db, kPacketUserId, unit);
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

void FillEvenSpread(const drogon::orm::DbClientPtr& db,
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

    auto c = QueryCapacity(db);
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
                GimuServer::InsertUnitFromMst(db, kPacketUserId, *unit);
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

void DoAddMat(const drogon::orm::DbClientPtr& db)
{
    // kind 2 = evo-material fodder (Pot / Bulb / Idol / Nymph lines).
    // kind 3 = special evo materials (5 light-element units); include both.
    FillEvenSpread(db,
        [](const UnitMst& u) { return u.kind == 2 || u.kind == 3; },
        "material");
}

void DoAddEnh(const drogon::orm::DbClientPtr& db)
{
    // kind 1 = XP-feeder / enhancer units (Metal Ghost / King / God lines,
    // Omni Frog / Omni Emperor, etc.).
    FillEvenSpread(db,
        [](const UnitMst& u) { return u.kind == 1; },
        "enhancer");
}

void DoAddElement(const drogon::orm::DbClientPtr& db, int32_t elementId)
{
    // Fill with mat (kind 2/3) AND enh (kind 1) units of the chosen element.
    FillEvenSpread(db,
        [elementId](const UnitMst& u) {
            return u.element == elementId && (u.kind == 1 || u.kind == 2 || u.kind == 3);
        },
        "element");
}

void DoClearUnits(const drogon::orm::DbClientPtr& db)
{
    db->execSqlSync(
        "DELETE FROM user_units WHERE user_id=$1 "
        "AND id <> (SELECT MIN(id) FROM user_units WHERE user_id=$1);",
        std::string(kPacketUserId));
    auto c = QueryCapacity(db);
    std::cout << "  Cleared. Now at " << c.cur << "/" << c.cap << ".\n";
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
        auto c = QueryCapacity(db);
        std::cout << "  " << c.cur << "/" << c.cap << " (" << c.free << " free)\n";
    }
    else if (cmd == "zel" || cmd == "karma" || cmd == "gems")
    {
        std::string arg; int64_t v = 0;
        if (!(iss >> arg) || !ParseInt(arg, v))
            { std::cout << "  Usage: " << cmd << " <n>\n"; return; }
        const std::string col = (cmd == "gems") ? "free_gems" : cmd;
        const int64_t cap = (cmd == "zel" || cmd == "karma") ? kMaxZelKarma
                                                              : std::numeric_limits<int64_t>::max();
        DoSetIntColumn(db, col, v, cmd, cap);
    }
    else if (cmd == "addunit")
    {
        std::string sub;
        if (!(iss >> sub)) { std::cout << "  Usage: addunit <mst_id|all|mat|enh|fire|water|earth|thunder|light|dark>\n"; return; }
        if      (sub == "all") DoAddAll(db);
        else if (sub == "mat") DoAddMat(db);
        else if (sub == "enh") DoAddEnh(db);
        else if (int32_t elem = ParseElement(sub); elem != 0) DoAddElement(db, elem);
        else
        {
            int64_t id = 0;
            if (!ParseInt(sub, id))
                { std::cout << "  Unknown subcommand '" << sub << "'.\n"; return; }
            DoAddUnit(db, static_cast<int32_t>(id));
        }
    }
    else if (cmd == "clearunits")
    {
        DoClearUnits(db);
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
