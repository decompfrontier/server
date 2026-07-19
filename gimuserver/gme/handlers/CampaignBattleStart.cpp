#include "App.hpp"
#include "Handlers.hpp"

#include <gimuserver/gme/common/Common.hpp>

#include <ctime>

// CampaignBattleStart (h1RjcD3S) — pre-battle request fired when the player
// confirms their deck and taps "Battle".  Persists the active mission ID so
// CampaignBattleEnd knows which mission row to update.  The client renders the
// battle entirely from its local stage/MST data; the server response only needs
// to not close the session (empty {} is sufficient for now — flesh out once a
// real capture is available).
//
// CampaignBattleStartReq (login_info + mission_id) is generated from the KDL
// (packet-generator/assets/net/handlers.kdl).

HANDLEF(CampaignBattleStart)
{
    LOG_INFO << "CampaignBattleStart: " << json;

    CampaignBattleStartReq req{};
    {
        glz::context ctx{};
        // Lenient parse — request carries IKqx1Cn9 + other unknown envelope keys.
        if (const auto ec = glz::read<glz::opts{.error_on_unknown_keys = false}>(req, json, ctx); ec)
        {
            LOG_WARN << "CampaignBattleStart: parse error: " << glz::format_error(ec, json);
        }
    }

    const auto identity = (co_await gme::getUserIdentity(theDb(), req.login_info)).nonEmpty();
    const std::string kUserId = identity.userId;

    // Persist active mission so BattleEnd can update the right row.
    if (!req.mission_id.empty())
    {
        try
        {
            co_await theDb()->execSqlCoro(
                "INSERT INTO user_campaign_state (user_id, active_mission_id)"
                " VALUES ($1,$2)"
                " ON CONFLICT(user_id) DO UPDATE SET active_mission_id=$2;",
                std::string(kUserId), req.mission_id);
        }
        catch (const drogon::orm::DrogonDbException& ex)
        {
            LOG_WARN << "CampaignBattleStart: state UPSERT failed: " << ex.base().what();
        }
    }

    // Reinforce slot — tojMy68W (FriendInfoResponse).  Mirrors the MissionStart
    // emission so the campaign squad-select friend picker is also populated.
    // Per the IDA audit (tools/ida/audits/tojMy68W_audit.txt) this feeds the
    // FriendInfoList singleton that the squad-select UI consults.  KDL schema
    // is in packet-generator/assets/net/friends.kdl.
    //
    // Stands in for the player's highest-level unit since there's no friend
    // system on the offline server.
    FriendInfo friend_entry{};
    bool       haveFriend = false;
    try
    {
        const auto reinforceRows = co_await theDb()->execSqlCoro(
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

        if (!reinforceRows.empty())
        {
            const auto& r = reinforceRows[0];
            const auto raw = r["unit_id"].as<std::string>();
            const auto sep = raw.find('_');
            int32_t mstId = 0;
            try { mstId = std::stoi(sep != std::string::npos ? raw.substr(0, sep) : raw); }
            catch (...) {}

            // FriendInfo.element is uint32_t — invert the string form stored
            // in user_units.element.
            const auto elemStr = r["element"].as<std::string>();
            int32_t elemId = 1;
            if      (elemStr == "fire")    elemId = 1;
            else if (elemStr == "water")   elemId = 2;
            else if (elemStr == "earth")   elemId = 3;
            else if (elemStr == "thunder") elemId = 4;
            else if (elemStr == "light")   elemId = 5;
            else if (elemStr == "dark")    elemId = 6;

            friend_entry.user_id            = "n9ZMPC0t";
            friend_entry.handle_name        = "DecompFriend";
            friend_entry.team_lv            = 999;
            friend_entry.friend_type        = 1;
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
            friend_entry.friend_id          = "DECOMP01";
            friend_entry.friend_message     = "GG WP";
            friend_entry.favorite           = 1;
            friend_entry.priority           = 1;
            friend_entry.deck_no            = 0;
            friend_entry.guild_id           = 0;

            haveFriend = true;
            LOG_INFO << "CampaignBattleStart: tojMy68W friend slot populated from unit "
                     << mstId << " (lv " << friend_entry.unit_lv << ")";
        }
    }
    catch (const drogon::orm::DrogonDbException& ex)
    {
        LOG_WARN << "CampaignBattleStart: friend query failed: " << ex.base().what();
    }

    if (!haveFriend)
    {
        // No units to source the friend from (fresh account / cleared
        // inventory) — preserve the original empty-OK response shape so
        // the client doesn't get confused.
        co_return HandleResult::success("{}");
    }

    std::string friendArrJson;
    if (const auto ec = glz::write_json(
            std::vector<FriendInfo>{friend_entry}, friendArrJson); ec)
    {
        LOG_WARN << "CampaignBattleStart: serialize tojMy68W: "
                 << glz::format_error(ec, friendArrJson);
        co_return HandleResult::success("{}");
    }

    std::string resp = R"({"tojMy68W":)";
    resp += friendArrJson;
    resp += '}';

    co_return HandleResult::success(resp);
}
