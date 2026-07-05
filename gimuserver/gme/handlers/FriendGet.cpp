#include "App.hpp"
#include "Handlers.hpp"

#include <gimuserver/gme/common/Common.hpp>

#include <ctime>

// FriendGet (2o4axPIC) — fires when the player opens the Reinforcement /
// "Choose a Helper" screen during squad selection (and also after
// MissionEnd to refresh the friend list for the next mission).
//
// Wire shape: the captured production response (BF-WorkingDir/server/deploy/
// log_res/2o4axPIC_*.json) emits {"xZH6EIQ7":[...]} — so the canonical
// response key is xZH6EIQ7 (populates ReinforcementInfoList).  Per the IDA
// dispatch table (tools/ida/audits/tojMy68W_audit.txt around line 114-117),
// the binary's master GameResponseParser::getResponseObject maps xZH6EIQ7 →
// ReinforcementInfoResponse → its readParam populates ReinforcementInfoList,
// which the picker is the only confirmed consumer of for the reinforce/
// helper UI.
//
// We also emit tojMy68W (FriendInfoResponse → FriendInfoList) with the same
// source data so any other UI that reads from FriendInfoList (general
// friend roster, friend-detail dialog) sees a populated list too.  Cheap to
// duplicate since both classes have nearly identical schemas; only the
// destination singleton differs.
//
// Request body carries a "StQIyohe":[{"jkldTrhL":"N"}] mode flag — observed
// values 0 and 2.  Probably toggles between full friend list and
// reinforce-eligible-only.  We currently ignore the mode and always return
// the same single fake friend.
//
// Offline-server stand-in: one entry sourced from the player's active-deck
// leader (fall back to highest-level unit).  When a curated pre-made friend
// set lands later, replace the single-row SQL with a loop emitting multiple
// entries to both arrays.

// Helper: convert user_units.element string ("fire"/"water"/...) to the
// integer element id FriendInfo / ReinforcementInfo expect.
static int32_t friendGet_elementToInt(const std::string& s)
{
    if (s == "fire")    return 1;
    if (s == "water")   return 2;
    if (s == "earth")   return 3;
    if (s == "thunder") return 4;
    if (s == "light")   return 5;
    if (s == "dark")    return 6;
    return 1;
}

HANDLEF(FriendGet)
{
    (void)session;
    LOG_INFO << "FriendGet: " << json;

    // Transitional bridge: resolve the sole offline user at runtime
    // (tutorial-created).  TODO port to gme::getUserIdentity.
    const std::string kUserId = co_await gme::getSoleUserId(theDb());

    FriendGetResp resp{};

    try
    {
        // Active-deck leader first, fall back to highest-level unit so the
        // friend slot always has data.  Matches MissionStart's query shape.
        auto rows = co_await theDb()->execSqlCoro(
            "SELECT uu.user_unit_id, uu.unit_id, uu.unit_lv,"
            " uu.base_hp,  uu.add_hp,  uu.ext_hp,"
            " uu.base_atk, uu.add_atk, uu.ext_atk,"
            " uu.base_def, uu.add_def, uu.ext_def,"
            " uu.base_heal,uu.add_heal,uu.ext_heal,"
            " uu.skill_id, uu.skill_lv, uu.extra_skill_id, uu.extra_skill_lv,"
            " uu.unit_type_id, uu.element"
            " FROM user_decks pd"
            " JOIN user_units uu ON uu.user_unit_id = pd.user_unit_id"
            " JOIN user_info ui ON ui.id = pd.user_id"
            " WHERE pd.user_id=$1 AND pd.deck_num=ui.active_deck AND pd.member_type=0"
            " LIMIT 1;",
            std::string(kUserId));

        if (rows.empty())
        {
            rows = co_await theDb()->execSqlCoro(
                "SELECT user_unit_id, unit_id, unit_lv,"
                " base_hp,  add_hp,  ext_hp,"
                " base_atk, add_atk, ext_atk,"
                " base_def, add_def, ext_def,"
                " base_heal,add_heal,ext_heal,"
                " skill_id, skill_lv, extra_skill_id, extra_skill_lv,"
                " unit_type_id, element"
                " FROM user_units"
                " WHERE user_id=$1"
                " ORDER BY unit_lv DESC, user_unit_id DESC LIMIT 1;",
                std::string(kUserId));
        }

        if (!rows.empty())
        {
            const auto& r = rows[0];

            // user_units.unit_id may carry "_100" suffix on evolved forms;
            // strip for the MST id on the wire.
            const auto raw = r["unit_id"].as<std::string>();
            const auto sep = raw.find('_');
            int32_t mstId = 0;
            try { mstId = std::stoi(sep != std::string::npos ? raw.substr(0, sep) : raw); }
            catch (...) {}

            const int32_t elemId         = friendGet_elementToInt(r["element"].as<std::string>());
            const int32_t unitLv         = r["unit_lv"].as<int32_t>();
            const int32_t baseHp         = r["base_hp"].as<int32_t>();
            const int32_t addHp          = r["add_hp"].as<int32_t>();
            const int32_t extHp          = r["ext_hp"].as<int32_t>();
            const int32_t baseAtk        = r["base_atk"].as<int32_t>();
            const int32_t addAtk         = r["add_atk"].as<int32_t>();
            const int32_t extAtk         = r["ext_atk"].as<int32_t>();
            const int32_t baseDef        = r["base_def"].as<int32_t>();
            const int32_t addDef         = r["add_def"].as<int32_t>();
            const int32_t extDef         = r["ext_def"].as<int32_t>();
            const int32_t baseHeal       = r["base_heal"].as<int32_t>();
            const int32_t addHeal        = r["add_heal"].as<int32_t>();
            const int32_t extHeal        = r["ext_heal"].as<int32_t>();
            const int32_t skillId        = r["skill_id"].as<int32_t>();
            const int32_t skillLv        = r["skill_lv"].as<int32_t>();
            const int32_t extraSkillId   = r["extra_skill_id"].as<int32_t>();
            const int32_t extraSkillLv   = r["extra_skill_lv"].as<int32_t>();
            const int32_t unitTypeId     = r["unit_type_id"].as<int32_t>();
            const int32_t loginTimestamp = static_cast<int32_t>(std::time(nullptr));

            // === ReinforcementInfo entry (xZH6EIQ7) — the production-canonical
            // shape for FriendGet responses.
            ReinforcementInfo ri{};
            ri.user_id            = "n9ZMPC0t";       // placeholder friend account id
            ri.handle_name        = "DecompFriend";
            ri.team_lv            = 999;
            ri.target_lv          = unitLv;
            ri.friend_type        = 1;                // 1 = friend (UNVERIFIED enum)
            ri.last_login_date    = loginTimestamp;
            ri.unit_id            = mstId;
            ri.base_hp            = baseHp;
            ri.add_hp             = addHp;
            ri.ext_hp             = extHp;
            ri.base_atk           = baseAtk;
            ri.add_atk            = addAtk;
            ri.ext_atk            = extAtk;
            ri.base_def           = baseDef;
            ri.add_def            = addDef;
            ri.ext_def            = extDef;
            ri.base_heal          = baseHeal;
            ri.add_heal           = addHeal;
            ri.ext_heal           = extHeal;
            ri.friend_point       = 0;
            ri.normal_friend_point = 0;
            ri.skill_id           = skillId;
            ri.skill_lv           = skillLv;
            ri.unit_type_id       = unitTypeId;
            ri.extra_skill_id     = extraSkillId;
            ri.extra_skill_lv     = extraSkillLv;
            ri.user_unit_id       = 999999;           // fake friend-row id
            ri.mission_id         = "";
            resp.reinforce_info.emplace_back(std::move(ri));

            // === FriendInfo entry (tojMy68W) — same data into FriendInfoList
            // for any UI consumer that reads from there.
            FriendInfo fi{};
            fi.user_id            = "n9ZMPC0t";
            fi.handle_name        = "DecompFriend";
            fi.team_lv            = 999;
            fi.friend_type        = 1;
            fi.last_login_date    = loginTimestamp;
            fi.unit_id            = mstId;
            fi.unit_lv            = unitLv;
            fi.base_hp            = baseHp;
            fi.add_hp             = addHp;
            fi.ext_hp             = extHp;
            fi.base_atk           = baseAtk;
            fi.add_atk            = addAtk;
            fi.ext_atk            = extAtk;
            fi.base_def           = baseDef;
            fi.add_def            = addDef;
            fi.ext_def            = extDef;
            fi.base_heal          = baseHeal;
            fi.add_heal           = addHeal;
            fi.ext_heal           = extHeal;
            fi.skill_id           = std::to_string(skillId);
            fi.skill_lv           = skillLv;
            fi.extra_skill_id     = std::to_string(extraSkillId);
            fi.extra_skill_lv     = extraSkillLv;
            fi.unit_type_id       = unitTypeId;
            fi.element            = elemId;
            fi.friend_id          = "DECOMP01";
            fi.friend_message     = "GG WP";
            fi.favorite           = 1;
            fi.priority           = 1;
            fi.deck_no            = 0;
            fi.guild_id           = 0;
            resp.friend_info.emplace_back(std::move(fi));

            LOG_INFO << "FriendGet: returning 1 friend (unit " << mstId
                     << " lv " << unitLv << ") under both xZH6EIQ7 and tojMy68W";
        }
        else
        {
            LOG_WARN << "FriendGet: no units in inventory — returning empty friend list";
        }
    }
    catch (const drogon::orm::DrogonDbException& ex)
    {
        LOG_WARN << "FriendGet: friend query failed: " << ex.base().what();
    }

    std::string buffer{};
    if (const auto& ec = glz::write_json(resp, buffer); ec)
    {
        const auto& glze = glz::format_error(ec, buffer);
        LOG_ERROR << "FriendGet: serialization error: " << glze;
        co_return HandleResult::error("Serialization error", glze);
    }

    co_return HandleResult::success(buffer);
}
