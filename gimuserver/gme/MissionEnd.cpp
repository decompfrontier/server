#include "App.hpp"
#include "Handlers.hpp"

#include <random>
#include <set>

// MissionEnd (9TvyNR5H) — fired by the client after the battle-result screen
// is acknowledged.  Grants fixed rewards, persists them to DB, and returns an
// updated UserTeamInfo so the home-screen HUD reflects the new zel/karma/exp.
//
// Response keys:
//   "fEi17cnx" — [UserTeamInfo]  — refreshes zel / karma / exp in HUD
//   "UT1SVg59" — { "h7eY3sAK": user_id }  — UserClearMissionInfo stub
//
// TODO: replace fixed rewards with F_MISSION_MST per-mission reward lookup
// (zel/karma/exp + drop chance fields keyed on mission_id).  Pairs with
// the MissionStart and CampaignBattleEnd MST loading TODOs.
static constexpr int64_t kMissionZelReward   = 500;
static constexpr int64_t kMissionKarmaReward = 100;
static constexpr int64_t kMissionExpReward   = 100;

// Wraps UserTeamInfo under the "fEi17cnx" single-element array key.
struct MeTeamWrapper {
    UserTeamInfo team_info = {};
};
template<> struct glz::meta<MeTeamWrapper> {
    using T = MeTeamWrapper;
    static constexpr auto value = glz::object(
        "fEi17cnx", pkg::glaze::single_array<&T::team_info>()
    );
};

// Mirrors GimuServer::ElementIdToString / unitEvo_elementStr.  Kept local to
// avoid a cross-TU dependency just for a six-entry switch.
static std::string missionEnd_elementStr(int32_t e)
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

// Parses req.battle_result.unit_drops into a list of MST ids.
// Wire format is "<mst_id>:<rarity>:<flag>,<mst_id>:<rarity>:<flag>,..."
// (e.g. "30030:1:1,10030:1:1,50030:2:2").  We only need the first value;
// rarity/flag are derivable from F_UNIT_MST so we don't trust the client to
// supply them.
static std::vector<int32_t> missionEnd_parseUnitDrops(std::string_view raw)
{
    std::vector<int32_t> ids;
    size_t i = 0;
    while (i < raw.size())
    {
        const size_t idStart = i;
        while (i < raw.size() && raw[i] != ':' && raw[i] != ',') ++i;
        const auto tok = raw.substr(idStart, i - idStart);
        if (!tok.empty())
        {
            try { ids.push_back(std::stoi(std::string(tok))); }
            catch (...) { /* skip malformed entry */ }
        }
        // Advance past rarity:flag (if present) to the next ','.
        while (i < raw.size() && raw[i] != ',') ++i;
        if (i < raw.size() && raw[i] == ',') ++i;
    }
    return ids;
}

// ── Build UserTeamInfo from a fresh DB row ────────────────────────────────────
static UserTeamInfo missionEnd_buildTeamInfo(
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

    // action_rest_timer: when energy < max, the BF client shows the count-down
    // text "Recovers in M:SS" and disables the "+" gem-refill nag.  When the
    // timer is 0, the client unconditionally renders "Energy is full." — so
    // leaving it at the struct default 0 made the HUD say "full" even when
    // the bar showed 348/398 (server hadn't implemented passive recharge but
    // the visible energy was below max because MissionStart deducted N).
    //
    // Setting it to a positive placeholder makes the HUD render a countdown
    // and stops the "full" text from appearing.  Real BF refills 1 energy
    // per ~3 minutes (180s) — observed empirically during user testing on
    // 2026-05-13 (earlier "5 minutes" recollection was wrong; correct value
    // appears to be 3 minutes).  Recharge ticking isn't implemented on the
    // server yet, so we hand the client a static 180 and the client's local
    // countdown ticks it down to 0; once at 0 the bar should bump +1 (the
    // client may handle the increment locally too, or wait for a server
    // refresh).
    // TODO: implement a real energy-regen scheduler that catches up energy
    // based on (now - last_energy_tick) when the player logs back in or
    // calls UserInfo / MissionStart.  Once that lands we send the
    // remaining seconds until the next tick (i.e. 180 - ((now -
    // last_energy_tick) % 180)).
    if (ti.action_point < ti.max_action_point)
        ti.action_rest_timer = 180;  // client recharges 1 energy per 3 min

    return ti;
}

// ── Handler ──────────────────────────────────────────────────────────────────

HANDLEF(MissionEnd)
{
    LOG_INFO << "MissionEnd: " << json;

    static constexpr std::string_view kUserId = "0839899613932562";

    // Step 1: parse the request using the KDL-generated MissionEndReq
    // struct.  All 41 rXvA1E5y fields, the 0xgIw1Ns deck array, and the
    // Kz7qfSs5 mission-info wrapper are declared in
    // packet-generator/assets/net/mission.kdl, so the lenient glaze read
    // no longer has to skip 40+ unknown sibling keys (handbook §3.1
    // silent-parse-fail risk).
    //
    // Replaces the previous substring-extraction hack — we now have typed
    // access to every field the client sends, including item_drops,
    // unit_drops, monsters_killed, etc.  These remain TODOs to act on
    // (item/unit inventory tables aren't wired yet) but the parse is now
    // structurally complete.
    //
    // The client doesn't ship an exp field anywhere in rXvA1E5y (no
    // d96tuT2E / gQInj3H6 / BattleExp setter anywhere in IDA exports for
    // this group), so exp stays on the hardcoded kMissionExpReward
    // placeholder until F_MISSION_MST is decoded and exp can be looked up
    // server-side by mission_id.
    static constexpr int64_t kCap = 99'999'999;

    ::MissionEndReq req{};
    if (const auto ec = glz::read<glz::opts{.error_on_unknown_keys = false}>(req, json); ec)
        LOG_WARN << "MissionEnd: parse warning: " << glz::format_error(ec, json);

    // Fall back to the hardcoded placeholders only when the request didn't
    // ship the field at all (parse failed → both stay 0).  earn_zel /
    // earn_karma are int32_t on the wire; widen to int64_t for the SQL
    // bind to match userinfo column types.
    int64_t zelReward   = (req.battle_result.earn_zel != 0)
                          ? static_cast<int64_t>(req.battle_result.earn_zel)
                          : kMissionZelReward;
    int64_t karmaReward = (req.battle_result.earn_karma != 0)
                          ? static_cast<int64_t>(req.battle_result.earn_karma)
                          : kMissionKarmaReward;

    LOG_INFO << "MissionEnd: crediting zel=" << zelReward
             << " karma=" << karmaReward
             << " exp=" << kMissionExpReward
             << " (BC=" << req.battle_result.battle_crystal_num
             << " HC=" << req.battle_result.heart_crystal_num
             << " max_turn_damage=" << req.battle_result.max_turn_damage
             << ")";
    // Unit drops are credited below (Step 1c).  BC/HC/item_drops are still
    // TODO — they need a user_brave_crystals counter and a user_items table
    // respectively, neither of which is wired up yet.

    // Step 2: credit rewards via three narrow single-column UPDATEs.
    //
    // The combined `zel = MIN(zel+$2, kCap), karma = MIN(...), exp = ...`
    // shape used to live here and silently failed (DB exp stuck at the seed
    // value across multiple completed missions) — same Drogon `$N`-binding
    // gotcha MissionStart hit when literals lived inside the SET expression.
    // Splitting into one UPDATE per column with the cap parameterised matches
    // the pattern other working handlers (UnitMix, UnitEvo) use.
    try
    {
        co_await theDb()->execSqlCoro(
            "UPDATE userinfo SET zel = MIN(zel + $1, $2) WHERE id=$3;",
            zelReward, kCap, std::string(kUserId));
    }
    catch (const drogon::orm::DrogonDbException& ex)
    {
        LOG_WARN << "MissionEnd: zel UPDATE failed: " << ex.base().what();
    }

    try
    {
        co_await theDb()->execSqlCoro(
            "UPDATE userinfo SET karma = MIN(karma + $1, $2) WHERE id=$3;",
            karmaReward, kCap, std::string(kUserId));
    }
    catch (const drogon::orm::DrogonDbException& ex)
    {
        LOG_WARN << "MissionEnd: karma UPDATE failed: " << ex.base().what();
    }

    try
    {
        co_await theDb()->execSqlCoro(
            "UPDATE userinfo SET exp = exp + $1 WHERE id=$2;",
            kMissionExpReward, std::string(kUserId));
    }
    catch (const drogon::orm::DrogonDbException& ex)
    {
        LOG_WARN << "MissionEnd: exp UPDATE failed: " << ex.base().what();
    }

    // Step 1b: level-up check (PER-LEVEL CHUNK interpretation).
    //
    // d96tuT2E in user_level_mst.json is the EXP required to GAIN AT level N-1
    // to reach level N — a per-level chunk, NOT a cumulative threshold.
    // The values stay roughly flat across levels (lv 901 chunk = 1009853,
    // lv 902 chunk = 1010025 — small inter-row deltas come from the
    // accidental shape of the curve, not from cumulative-ness).
    //
    // Confirming evidence from user testing:
    //   * Seed userinfo.exp = 1009680 is meant to leave the player
    //     "almost ready to level up" — bar reads ~99% full of the
    //     1009853 chunk for lv 901.
    //   * "XP to next Lv" at endgame is expected to be ~1M (the chunk
    //     for the next level), not ~170 (the delta between consecutive
    //     row values).  Under cumulative interpretation the display
    //     showed ~145, which the player flagged as obviously wrong.
    //
    // Correct loop: while current exp >= chunk required for the next
    // level, subtract that chunk and increment the level.  After the
    // loop, userinfo.exp holds the residual "progress at the new level"
    // that carries over.
    //
    // origLevel / newLevel are function-scoped so the F5Vs19mb block
    // below can populate BeforeLv (81CsGAR9) + LvupFlg (wPb5uMR9) for the
    // client's level-up animation — see that block for the cascade-control
    // semantics decoded from MissionRewardResponse::readParam.
    int32_t origLevel = 900;   // pre-mission level (default if SELECT fails)
    int32_t newLevel  = 900;   // post-mission level (after level-up check)
    int32_t newMaxAP  = 398;   // new level's max_action_point
    int64_t newExp    = 0;     // residual exp at new level (after chunk subtraction)

    try
    {
        const auto expRows = co_await theDb()->execSqlCoro(
            "SELECT level, exp FROM userinfo WHERE id=$1;",
            std::string(kUserId));
        if (!expRows.empty())
        {
            origLevel = expRows[0]["level"].as<int32_t>();
            newLevel  = origLevel;
            newExp    = expRows[0]["exp"].as<int64_t>();

            const auto& progression =
                theServer()->cache().initializeResp().progression;

            auto chunkFor = [&](int32_t lv) -> int64_t {
                for (const auto& e : progression)
                    if (e.level == lv) return static_cast<int64_t>(e.exp);
                return 0;  // 0 = past max level — terminates the loop
            };
            auto maxAPFor = [&](int32_t lv) -> int32_t {
                for (const auto& e : progression)
                    if (e.level == lv) return e.action_points;
                return 100;
            };

            while (true)
            {
                const int64_t need = chunkFor(newLevel + 1);
                if (need <= 0 || newExp < need) break;
                newExp  -= need;
                newLevel += 1;
            }

            if (newLevel != origLevel)
            {
                newMaxAP = maxAPFor(newLevel);
                co_await theDb()->execSqlCoro(
                    "UPDATE userinfo SET level=$1, exp=$2, energy=$3 WHERE id=$4;",
                    newLevel, newExp, newMaxAP, std::string(kUserId));
                LOG_INFO << "MissionEnd: level " << origLevel << " -> " << newLevel
                         << " (residual exp=" << newExp
                         << ", energy refilled to " << newMaxAP << ")";
            }
        }
    }
    catch (const drogon::orm::DrogonDbException& ex)
    {
        LOG_WARN << "MissionEnd: level-up check failed: " << ex.base().what();
    }

    // Step 1c: credit unit drops.  Parse the comma-separated triples from the
    // request, insert one user_units row per drop seeded from F_UNIT_MST, and
    // collect the resulting UserUnitInfo entries for the qC2tJs4E push below.
    //
    // The qC2tJs4E array is the same incremental-inventory channel
    // UnitMix/UnitEvo use to tell the client "a unit was added".  The client
    // appears to drive the result-screen unit-drop card animation off that
    // push — F5Vs19mb.RewardUnits alone only populates summary text.
    //
    // We trust only the unit_id (first triple slot) and re-derive stats from
    // F_UNIT_MST, so the client can't smuggle in fabricated drops.  Triples
    // for ids not present in UnitMst are dropped with a warning.
    //
    // Inventory cap is not yet enforced — userinfo.max_unit_count is 4000 on
    // the dev account so we'd hit a real BF gift-box overflow only after
    // thousands of unhandled drops.  TODO once a gift_box table exists.
    std::string droppedUnitsJson;
    // Dictionary entries for first-encounter drops — drives the "Unit
    // Discovered" / encyclopedia-unlock screen.  Wire schema is the
    // UserUnitDictionaryResponse (response_key GV81ctzR, params
    // [h7eY3sAK, pn16CNah, 2pAyFjmZ] per response_mappings.json).  Only the
    // UserID setter is reached via typed readParam at 0x1404abc; pn16CNah
    // (unit_id) and 2pAyFjmZ (UnitImgType) come through the hand-coded
    // getValue() path so they're not in readparam_analysis but they are
    // listed in the params catalog and must be emitted.
    std::string dictionaryJson;
    {
        const auto dropIds = missionEnd_parseUnitDrops(req.battle_result.unit_drops);
        if (!dropIds.empty())
        {
            const auto& unitMst = theServer()->cache().unitMst();

            struct DropResolved {
                const UnitMst* mst;
                int32_t        unit_type_id;
                int32_t        skill_lv;
                int32_t        extra_skill_lv;
                std::string    element;
            };
            std::vector<DropResolved> resolved;
            resolved.reserve(dropIds.size());

            static thread_local std::mt19937 rng(std::random_device{}());
            std::uniform_int_distribution<int32_t> typeDist(1, 6);

            for (int32_t id : dropIds)
            {
                const UnitMst* mst = nullptr;
                for (const auto& u : unitMst) { if (u.id == id) { mst = &u; break; } }
                if (!mst)
                {
                    LOG_WARN << "MissionEnd: unit_drop id " << id
                             << " not in UnitMst, skipping";
                    continue;
                }
                DropResolved r{};
                r.mst            = mst;
                r.unit_type_id   = typeDist(rng);
                r.skill_lv       = (mst->skill_id       > 0) ? 10 : 0;
                r.extra_skill_lv = (mst->extra_skill_id > 0) ? 10 : 0;
                r.element        = missionEnd_elementStr(mst->element);
                resolved.emplace_back(std::move(r));
            }

            if (!resolved.empty())
            {
                // Pre-INSERT inventory check: find which of the resolved MST
                // ids the user already owns (in any form, including evolved
                // _100 variants).  Anything NOT in this set is a first
                // encounter and triggers the dictionary-unlock screen via
                // the GV81ctzR push appended to the response below.
                //
                // The query pulls every owned unit_id rather than filtering
                // server-side because the dev account's inventory is small
                // (<= max_unit_count of 4000) and the column is TEXT — the
                // suffix-stripping is cleaner in C++ than in an IN-clause
                // with both bare and _suffix variants per id.
                std::set<int32_t> alreadyOwned;
                try
                {
                    const auto seenRows = co_await theDb()->execSqlCoro(
                        "SELECT DISTINCT unit_id FROM user_units WHERE user_id=$1;",
                        std::string(kUserId));
                    for (const auto& row : seenRows)
                    {
                        const std::string s = row["unit_id"].as<std::string>();
                        const auto pos = s.find('_');
                        const auto base = (pos != std::string::npos) ? s.substr(0, pos) : s;
                        try { alreadyOwned.insert(std::stoi(base)); }
                        catch (...) { /* skip non-numeric ids */ }
                    }
                }
                catch (const drogon::orm::DrogonDbException& ex)
                {
                    LOG_WARN << "MissionEnd: pre-INSERT inventory check failed: "
                             << ex.base().what();
                }

                // One batched INSERT — same Drogon-SQLite-worker-thread
                // mitigation as GachaAction (handbook §6.14).  Integer cols
                // are inlined from trusted UnitMst+RNG; only the user_id
                // string is parameter-bound.
                std::string sql =
                    "INSERT INTO user_units "
                    "(user_id, unit_id, unit_lv,"
                    " base_hp,  add_hp,  ext_hp,  limit_over_hp,"
                    " base_atk, add_atk, ext_atk, limit_over_atk,"
                    " base_def, add_def, ext_def, limit_over_def,"
                    " base_heal,add_heal,ext_heal,limit_over_heal,"
                    " exp, total_exp,"
                    " skill_id, skill_lv, extra_skill_id, extra_skill_lv, leader_skill_id,"
                    " element, fe_bp, fe_max_usable_bp, unit_type_id) VALUES ";

                for (size_t i = 0; i < resolved.size(); ++i)
                {
                    const auto& r = resolved[i];
                    const auto& m = *r.mst;
                    if (i) sql += ',';
                    sql += "($1,'"; sql += std::to_string(m.id); sql += "',1,";
                    sql += std::to_string(m.min_hp);  sql += ",0,0,0,";
                    sql += std::to_string(m.min_atk); sql += ",0,0,0,";
                    sql += std::to_string(m.min_def); sql += ",0,0,0,";
                    sql += std::to_string(m.min_rec); sql += ",0,0,0,1,1,";
                    sql += std::to_string(m.skill_id);       sql += ',';
                    sql += std::to_string(r.skill_lv);       sql += ',';
                    sql += std::to_string(m.extra_skill_id); sql += ',';
                    sql += std::to_string(r.extra_skill_lv); sql += ',';
                    sql += std::to_string(m.leader_skill_id);
                    sql += ",'"; sql += r.element; sql += "',100,200,";
                    sql += std::to_string(r.unit_type_id);
                    sql += ')';
                }
                sql += ';';

                try
                {
                    co_await theDb()->execSqlCoro(sql, std::string(kUserId));

                    // SQLite increments rowid monotonically within a single
                    // INSERT statement, so the N rows just inserted have ids
                    // last-N+1 .. last.
                    const auto idRows = co_await theDb()->execSqlCoro(
                        "SELECT last_insert_rowid() AS id;");
                    if (idRows.empty())
                    {
                        LOG_ERROR << "MissionEnd: last_insert_rowid returned no row after unit-drop INSERT";
                    }
                    else
                    {
                        const int32_t lastId  = idRows[0]["id"].as<int32_t>();
                        const int32_t firstId = lastId - static_cast<int32_t>(resolved.size()) + 1;

                        std::vector<UserUnitInfo> drops;
                        drops.reserve(resolved.size());
                        for (size_t i = 0; i < resolved.size(); ++i)
                        {
                            const auto& r = resolved[i];
                            const auto& m = *r.mst;
                            UserUnitInfo u = {};
                            u.user_id          = std::string(kUserId);
                            u.user_unit_id     = firstId + static_cast<int32_t>(i);
                            u.unit_id          = m.id;
                            u.unit_type_id     = r.unit_type_id;
                            u.unit_lv          = 1;
                            u.exp              = 1;
                            u.total_exp        = 1;
                            u.base_hp          = m.min_hp;
                            u.base_atk         = m.min_atk;
                            u.base_def         = m.min_def;
                            u.base_heal        = m.min_rec;
                            u.element          = r.element;
                            u.leader_skill_id  = m.leader_skill_id;
                            u.skill_id         = m.skill_id;
                            u.skill_lv         = r.skill_lv;
                            u.extra_skill_id   = m.extra_skill_id;
                            u.extra_skill_lv   = r.extra_skill_lv;
                            u.fe_bp            = 100;
                            u.fe_max_usable_bp = 200;
                            u.new_flag         = true;
                            drops.emplace_back(std::move(u));
                        }

                        // Serialize each UserUnitInfo via glaze so we don't
                        // have to escape strings/types by hand, then join
                        // with commas.  This mirrors the string-append style
                        // the rest of MissionEnd uses for UT1SVg59 / F5Vs19mb.
                        for (size_t i = 0; i < drops.size(); ++i)
                        {
                            std::string item;
                            if (const auto ec = glz::write_json(drops[i], item); ec)
                            {
                                LOG_ERROR << "MissionEnd: serialize UserUnitInfo[" << i
                                          << "]: " << glz::format_error(ec, item);
                                continue;
                            }
                            if (!droppedUnitsJson.empty()) droppedUnitsJson += ',';
                            droppedUnitsJson += item;
                        }

                        LOG_INFO << "MissionEnd: credited " << drops.size()
                                 << " unit drop(s); ids " << firstId << ".." << lastId;

                        // Build dictionary-unlock entries for MST ids that
                        // weren't already in the user's inventory pre-INSERT.
                        // Deduped across the drop set so two copies of the
                        // same unit only produce one dictionary entry.
                        std::set<int32_t> firstEncounterMstIds;
                        for (const auto& r : resolved)
                        {
                            if (alreadyOwned.find(r.mst->id) == alreadyOwned.end())
                                firstEncounterMstIds.insert(r.mst->id);
                        }
                        for (int32_t id : firstEncounterMstIds)
                        {
                            if (!dictionaryJson.empty()) dictionaryJson += ',';
                            dictionaryJson += R"({"h7eY3sAK":")";
                            dictionaryJson += std::string(kUserId);
                            dictionaryJson += R"(","pn16CNah":")";
                            dictionaryJson += std::to_string(id);
                            // 2pAyFjmZ = UnitImgType (uint32_t per readparam_analysis).
                            // Default 0 matches the typed handler's m_UnitImgType
                            // initial value; unit_img_type variants exist for
                            // omni/limit-break art but the base entry uses 0.
                            dictionaryJson += R"(","2pAyFjmZ":"0"})";
                        }
                        if (!firstEncounterMstIds.empty())
                        {
                            LOG_INFO << "MissionEnd: GV81ctzR first-encounter dictionary entries: "
                                     << firstEncounterMstIds.size();
                        }
                    }
                }
                catch (const drogon::orm::DrogonDbException& ex)
                {
                    LOG_WARN << "MissionEnd: unit-drop INSERT failed: " << ex.base().what();
                }
            }
        }
    }

    // Step 2: fetch fresh userinfo for the response.
    const auto infoRows = co_await theDb()->execSqlCoro(
        "SELECT level, exp, zel, karma, brave_coin, free_gems, paid_gems, energy,"
        " max_unit_count, max_warehouse_count, summon_tickets, rainbow_coins,"
        " colosseum_tickets, friend_point, total_brave_points, avail_brave_points,"
        " active_deck, want_gift FROM userinfo WHERE id=$1;",
        std::string(kUserId));

    if (infoRows.empty())
    {
        LOG_ERROR << "MissionEnd: userinfo not found";
        co_return HandleResult::success("{}");
    }

    // Step 3: build and serialize fEi17cnx.
    MeTeamWrapper tw{};
    tw.team_info = missionEnd_buildTeamInfo(
        infoRows[0],
        theServer()->cache().initializeResp().progression);

    std::string teamJson{};
    if (const auto ec = glz::write_json(tw, teamJson); ec)
    {
        LOG_ERROR << "MissionEnd: serialize teamInfo: " << glz::format_error(ec, teamJson);
        co_return HandleResult::success("{}");
    }

    // Step 4: append UT1SVg59 (UserClearMissionInfo) and F5Vs19mb
    // (MissionRewardResponse — the "Quest Clear Reward" screen).
    //
    // Full field map decoded from the IDA pseudocode of
    // MissionRewardResponse::readParam in libgame.so (audit script:
    // f5vs19mb_field_audit.py, output to <idb>.f5vs19mb_audit.txt):
    //
    //   92ms7J5M -> setClearBonus       (string) first-clear bonus flag
    //   wPb5uMR9 -> setLvupFlg          (int)    0=no anim, !=0 plays anim
    //   81CsGAR9 -> setBeforeLv         (int)    level BEFORE this mission
    //   d96tuT2E -> setIncExp           (int)    "XP Obtained" displayed
    //   0egM9p7I -> setReinReqState     (int)    reinforce-request state
    //   4sQ8vBXm -> setClearDungeonID   (string) cleared dungeon ID
    //   NgPQbA46 -> setClearAreaID      (string) cleared area ID
    //   mauD5qZ1 -> setClearMissionID   (string) cleared mission ID
    //   3MAT6quo -> setRewardUnits      (string) dropped units list
    //   Najhr8m6 -> setZel              (int)    zel obtained
    //   HTVh8a65 -> setKarma            (int)    karma obtained
    //   3vXLQFjt -> setExpUpType        (string) exp-bonus indicator
    //   k6MynZkg -> setZelUpType        (string) zel-bonus indicator
    //   J1MwgxCL -> setKarmaUpType      (string) karma-bonus indicator
    //   b8D18b7e -> setExpGuild         (int)    guild exp bonus
    //   7adBa819 -> setKarmaGuild       (int)    guild karma bonus
    //   gzs3Bw10 -> setZelGuild         (int)    guild zel bonus
    //   03fFk30f -> setEToken           (int)    event token reward
    //   S2ChOKo6 -> setRepeatClearBonus (string) repeat-clear bonus
    //
    // The previous cascade-suppression hack (d96tuT2E=0) was wrong.  The
    // real cascade control is BeforeLv (81CsGAR9) and LvupFlg (wPb5uMR9):
    // when both equal the current level (no level-up), the client doesn't
    // animate.  When LvupFlg!=0 and BeforeLv < team_info.level, the
    // client animates BeforeLv -> team_info.level.  My old "all zeros"
    // sent BeforeLv=0 with team_info.level=900, hence the 0->900
    // animation that the user kept seeing.
    //
    // Now: origLevel = pre-mission level, newLevel = post-mission level
    // (both captured in the level-up check above).  We pass them through
    // so cascades only fire when the player actually crosses a threshold.
    if (teamJson.size() >= 2 && teamJson.back() == '}')
    {
        teamJson.pop_back();

        // UT1SVg59 — UserClearMissionInfo clear-history record per the
        // IDA audit (tools/ida/audits/UT1SVg59_audit.txt).  All 7 setters
        // are now known and string-typed.  Values below are placeholders
        // until we add a user_mission_clear table for tracking per-mission
        // stats (clear count, fastest time, lifetime BC/HC, last-cleared
        // date).
        //
        // Earlier we only sent h7eY3sAK; the other 6 fields stayed at
        // their server-side defaults.  Emitting them all means the
        // client's clear-history list ends up with a complete (if
        // placeholder-valued) entry rather than a stub.
        teamJson += R"(,"UT1SVg59":[{)";
        teamJson += R"("h7eY3sAK":"0839899613932562",)";  // setUserID
        teamJson += R"("j28VNcUW":"",)";   // setMissionID    (TODO: persist active_mission_id from MissionStart so MissionEnd knows which mission was just cleared)
        teamJson += R"("JATWnN57":"",)";   // setClearDate    (TODO: persist last-cleared timestamp)
        teamJson += R"("HIQoGb10":"1",)";  // setClearCnt     (TODO: real per-mission clear counter)
        teamJson += R"("Wi4EFe6f":"",)";   // setMostTime     (TODO: track fastest clear)
        teamJson += R"("RWX38boG":"0",)";  // setGetBc        (TODO: lifetime BC from this mission)
        teamJson += R"("bGx5ku0F":"0")";   // setGetHc        (TODO: lifetime HC from this mission)
        teamJson += R"(}])";

        // Mission ID parsed out of the request's mission_info group — the
        // result screen renders an "Area / Mission name" header above the
        // reward block, so this should be the mission that was just
        // played.  battle_serial is a session id (k9cxD7Ba), not the
        // mission_id — we don't have a server-side store of the active
        // mission yet (MissionStart doesn't persist it for Grand Gaia
        // missions; Campaign does via user_campaign_state).  Leave the
        // mission/area/dungeon IDs empty for now; the client will fall
        // back to its cached current-mission state.
        // TODO: persist active_mission_id from MissionStart so we can
        // populate mauD5qZ1 / NgPQbA46 / 4sQ8vBXm correctly here.

        const int32_t lvupFlg = (newLevel != origLevel) ? 1 : 0;

        // *UpType fields (3vXLQFjt / k6MynZkg / J1MwgxCL) are kept here
        // commented out for future restoration.  These hold the
        // "Experience x1!" / "Zel x1!" / "Karma x1!" bonus-multiplier
        // badge strings that surface during event campaigns.  Sending
        // them as empty strings caused the client to render a stray
        // "Experience x1!" placeholder badge over *all three* reward
        // fields (the empty-string default in the client's UI hits a
        // single shared "Experience x1!" texture).  Omitting the keys
        // entirely suppresses the badges — matches the no-event base
        // case — but leaves them ready to re-enable once we model
        // events.  Likely format (per the IDA setter signature std::string)
        // is "<RewardKind>/x<multiplier>" e.g. "Zel/x2".  Since this is
        // an offline server the player may want to crank these up for
        // easier grinding even without a real event system.
        teamJson += R"(,"F5Vs19mb":[{)";
        teamJson += R"("92ms7J5M":"",)";                                                                  // ClearBonus
        teamJson += R"("wPb5uMR9":")" + std::to_string(lvupFlg)            + R"(",)";                     // LvupFlg
        teamJson += R"("81CsGAR9":")" + std::to_string(origLevel)          + R"(",)";                     // BeforeLv
        teamJson += R"("d96tuT2E":")" + std::to_string(kMissionExpReward)  + R"(",)";                     // IncExp ("XP Obtained")
        teamJson += R"("0egM9p7I":"0",)";                                                                 // ReinReqState
        teamJson += R"("4sQ8vBXm":"",)";                                                                  // ClearDungeonID (TODO)
        teamJson += R"("NgPQbA46":"",)";                                                                  // ClearAreaID    (TODO)
        teamJson += R"("mauD5qZ1":"",)";                                                                  // ClearMissionID (TODO)
        teamJson += R"("3MAT6quo":")" + req.battle_result.unit_drops       + R"(",)";                     // RewardUnits (echoed for summary text; the "you obtained unit X" animation is driven by the qC2tJs4E push below)
        teamJson += R"("Najhr8m6":")" + std::to_string(zelReward)          + R"(",)";                     // Zel
        teamJson += R"("HTVh8a65":")" + std::to_string(karmaReward)        + R"(",)";                     // Karma
        // teamJson += R"("3vXLQFjt":"",)";                                                               // ExpUpType   (event-multiplier badge — keep commented)
        // teamJson += R"("k6MynZkg":"",)";                                                               // ZelUpType   (event-multiplier badge — keep commented)
        // teamJson += R"("J1MwgxCL":"",)";                                                               // KarmaUpType (event-multiplier badge — keep commented)
        teamJson += R"("b8D18b7e":"0",)";                                                                 // ExpGuild
        teamJson += R"("7adBa819":"0",)";                                                                 // KarmaGuild
        teamJson += R"("gzs3Bw10":"0",)";                                                                 // ZelGuild
        teamJson += R"("03fFk30f":"0",)";                                                                 // EToken
        teamJson += R"("S2ChOKo6":"")";                                                                   // RepeatClearBonus
        teamJson += R"(}])";

        // qC2tJs4E — incremental UserUnitInfo push for every freshly-credited
        // unit drop.  Only emitted when at least one drop survived MST
        // validation; sending an empty array would harmlessly be a no-op but
        // we omit the key to match UnitMix/UnitEvo's behaviour (they only
        // emit when there's something to push).  The entries themselves were
        // serialized inline above via glz::write_json on each UserUnitInfo.
        if (!droppedUnitsJson.empty())
        {
            teamJson += R"(,"qC2tJs4E":[)";
            teamJson += droppedUnitsJson;
            teamJson += ']';
        }

        // GV81ctzR — UserUnitDictionaryResponse.  One entry per MST id the
        // user just encountered for the first time (i.e. didn't already own
        // a copy of).  Drives the post-mission "Unit Discovered" / dictionary-
        // unlock screen.  Per response_mappings.json the wire schema is
        // {h7eY3sAK, pn16CNah, 2pAyFjmZ}; the dictionary-unlock animation is
        // separate from the "obtained unit X" card (driven by qC2tJs4E above)
        // and from the result-screen summary text (driven by F5Vs19mb.3MAT6quo).
        if (!dictionaryJson.empty())
        {
            teamJson += R"(,"GV81ctzR":[)";
            teamJson += dictionaryJson;
            teamJson += ']';
        }

        teamJson += '}';
    }

    co_return HandleResult::success(teamJson);
}
