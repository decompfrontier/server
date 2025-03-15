#include "UserInfoHandler.hpp"
#include <db/DbMacro.hpp>
#include <core/Utils.hpp>
#include <core/System.hpp>
#include "gme/response/UserTeamInfo.hpp"
#include "gme/response/UserLoginCampaignInfo.hpp"
#include "gme/response/NoticeInfo.hpp"
#include "gme/response/UserItemDictionaryInfo.hpp"
#include "gme/response/UserTeamArchive.hpp"
#include "gme/response/UserUnitInfo.hpp"
#include "gme/response/UserPartyDeckInfo.hpp"
#include "gme/response/UserWarehouseInfo.hpp"
#include "gme/response/UserClearMissionInfo.hpp"
#include "gme/response/ItemFavorite.hpp"
#include "gme/response/UserItemDictionaryInfo.hpp"
#include "gme/response/UserTeamArenaArchive.hpp"
#include "gme/response/UserUnitDictionary.hpp"
#include "gme/response/UserFavorite.hpp"
#include "gme/response/UserArenaInfo.hpp"
#include "gme/response/UserGiftInfo.hpp"
#include "gme/response/VideoAdInfo.hpp"
#include "gme/response/VideoAdRegion.hpp"
#include "gme/response/SummonerJournalUserInfo.hpp"
#include "gme/response/SignalKey.hpp"
#include <json/reader.h>

void Handler::UserInfoHandler::Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const
{
    // Extract and log the initial user ID
    std::string requestUserId = user.info.userID;
    LOG_INFO << "UserInfoHandler: Handling request for user_id (initial): " << requestUserId;

    // Check for user_id in the request body
    if (req.isMember("user_id")) {
        requestUserId = req["user_id"].asString();
        user.info.userID = requestUserId;
        LOG_INFO << "UserInfoHandler: Overriding user_id from request body to: " << requestUserId;
    }
    else if (req.isMember("ak")) { // Check for ak parameter (from login context)
        requestUserId = req["ak"].asString();
        user.info.userID = requestUserId;
        LOG_INFO << "UserInfoHandler: Overriding user_id from request ak to: " << requestUserId;
    }
    else {
        // Fallback: Use the user_id from the login response (hardcode for now)
        requestUserId = "0839899613932562";
        user.info.userID = requestUserId;
        LOG_INFO << "UserInfoHandler: No user_id or ak in request, using hardcoded user_id: " << requestUserId;
    }

    // Check if this is a unit inventory or squad management request
    std::string action = req.isMember("action") ? req["action"].asString() : "";
    if (action == "load_unit_inventory" || action == "Zw3WIoWu" || action == "load_squad_management") {
        GME_DB->execSqlAsync(
            "SELECT unit_id, name, rarity, element, stats, bb, leader_skill, ai FROM user_units WHERE user_id = $1 LIMIT 500",  // LIMIT 1470 here
            [this, &user, cb, action](const drogon::orm::Result& unitResult) {
            LOG_INFO << "UserInfoHandler: Found " << unitResult.size() << " units for user_id: " << user.info.userID;

            Json::Value res;
            Response::UserUnitInfo unitInfo;
            for (const auto& row : unitResult) {
                Response::UserUnitInfo::Data d;
                d.userID = user.info.userID;
                d.userUnitID = std::stoul(row["unit_id"].as<std::string>());
                d.unitID = std::stoul(row["unit_id"].as<std::string>());
                d.unitTypeID = row["rarity"].as<uint32_t>();
                d.element = row["element"].as<std::string>();
                d.unitLv = 1;
                d.newFlg = 1;
                d.receiveDate = 100;
                d.FeBP = 100;
                d.FeMaxUsableBP = 200;

                Json::Value stats;
                Json::Reader reader;
                std::string statsStr = row["stats"].as<std::string>();
                if (!reader.parse(statsStr, stats)) {
                    LOG_ERROR << "Failed to parse stats JSON: " << statsStr;
                    stats = Json::Value();
                }
                d.baseHp = stats.isMember("hp") ? stats["hp"].asUInt() : 1000;
                d.baseAtk = stats.isMember("atk") ? stats["atk"].asUInt() : 1000;
                d.baseDef = stats.isMember("def") ? stats["def"].asUInt() : 1000;
                d.baseHeal = stats.isMember("rec") ? stats["rec"].asUInt() : 1000;
                d.addHp = d.baseHp / 10;
                d.addAtk = d.baseAtk / 10;
                d.addDef = d.baseDef / 10;
                d.addHeal = d.baseHeal / 10;
                d.extHp = d.addHp;
                d.extAtk = d.addAtk;
                d.extDef = d.addDef;
                d.extHeal = d.addHeal;
                d.limitOverHP = d.baseHp / 5;
                d.limitOverAtk = d.baseAtk / 5;
                d.limitOverDef = d.baseDef / 5;
                d.limitOverHeal = d.baseHeal / 5;
                d.exp = 1;
                d.totalExp = 1;

                unitInfo.Mst.emplace_back(d);
            }

            // Fallback: Add a dummy unit if inventory is empty (to prevent crashes)
            if (unitInfo.Mst.empty()) {
                LOG_WARN << "UserInfoHandler: No units found, adding dummy unit for compatibility";
                Response::UserUnitInfo::Data d;
                d.userID = user.info.userID;
                d.userUnitID = 9999;
                d.unitID = 9999;
                d.unitTypeID = 1;
                d.element = "fire";
                d.unitLv = 1;
                d.newFlg = 1;
                d.receiveDate = 100;
                d.FeBP = 100;
                d.FeMaxUsableBP = 200;
                d.baseHp = 1000;
                d.baseAtk = 1000;
                d.baseDef = 1000;
                d.baseHeal = 1000;
                d.addHp = 100;
                d.addAtk = 100;
                d.addDef = 100;
                d.addHeal = 100;
                d.extHp = 100;
                d.extAtk = 100;
                d.extDef = 100;
                d.extHeal = 100;
                d.limitOverHP = 200;
                d.limitOverAtk = 200;
                d.limitOverDef = 200;
                d.limitOverHeal = 200;
                d.exp = 1;
                d.totalExp = 1;
                unitInfo.Mst.emplace_back(d);
            }

            unitInfo.Serialize(res);

            // Initialize party with the first unit to avoid Empty_Party_Info
            Response::UserPartyDeckInfo deckInfo;
            if (!unitInfo.Mst.empty()) {
                Response::UserPartyDeckInfo::Data d;
                d.deckNum = 0;
                d.deckType = 1;
                d.dispOrder = 0;
                d.memberType = 0; // Leader
                d.userUnitID = unitInfo.Mst[0].userUnitID;
                deckInfo.Mst.emplace_back(d);
            }
            deckInfo.Serialize(res);

            cb(newGmeOkResponse(GetGroupId(), GetAesKey(), res));
            return;
        },
            [this, cb](const drogon::orm::DrogonDbException& e) { OnError(e, cb); },
            user.info.userID
        );
        return;
    }

    // Original logic for initial load
    GME_DB->execSqlAsync(
        "SELECT level, exp, max_unit_count, max_friend_count, "
        "zel, karma, brave_coin, max_warehouse_count, want_gift, "
        "free_gems, paid_gems, active_deck, summon_tickets, "
        "rainbow_coins, colosseum_tickets, active_arena_deck, "
        "total_brave_points, avail_brave_points, energy "
        "FROM userinfo WHERE id = $1",
        [this, &user, cb](const drogon::orm::Result& result) {
        if (result.size() > 0) {
            int col = 0;
            auto& sql = result[0];
            user.teamInfo.UserID = user.info.userID;
            user.teamInfo.Level = sql[col++].as<uint32_t>();
            user.teamInfo.Exp = sql[col++].as<int64_t>();
            user.teamInfo.MaxUnitCount = sql[col++].as<uint32_t>();
            user.teamInfo.MaxFriendCount = sql[col++].as<uint32_t>();
            user.teamInfo.Zel = sql[col++].as<uint64_t>();
            user.teamInfo.Karma = sql[col++].as<uint64_t>();
            user.teamInfo.BraveCoin = sql[col++].as<uint32_t>();
            user.teamInfo.WarehouseCount = sql[col++].as<uint32_t>();
            user.teamInfo.WantGift = sql[col++].as<std::string>();
            user.teamInfo.FreeGems = sql[col++].as<uint32_t>();
            user.teamInfo.PaidGems = sql[col++].as<uint32_t>();
            user.teamInfo.ActiveDeck = sql[col++].as<uint32_t>();
            user.teamInfo.SummonTicket = sql[col++].as<uint32_t>();
            user.teamInfo.RainbowCoin = sql[col++].as<uint32_t>();
            user.teamInfo.ColosseumTicket = sql[col++].as<uint32_t>();
            user.teamInfo.ArenaDeckNum = sql[col++].as<uint32_t>();
            user.teamInfo.BravePointsTotal = sql[col++].as<uint32_t>();
            user.teamInfo.CurrentBravePoints = sql[col++].as<uint32_t>();
            user.teamInfo.ActionPoint = sql[col++].as<uint32_t>();
        }
        else {
            auto sc = System::Instance().MstConfig().StartInfo();
            user.teamInfo.UserID = user.info.userID;
            user.teamInfo.Level = sc.Level;
            user.teamInfo.Exp = 0;
            user.teamInfo.MaxUnitCount = 2292; // Updated to match unit count
            user.teamInfo.MaxFriendCount = sc.FriendCount;
            user.teamInfo.Zel = sc.Zel;
            user.teamInfo.Karma = sc.Karma;
            user.teamInfo.BraveCoin = 0;
            user.teamInfo.WarehouseCount = 2292; // Updated to match unit count
            user.teamInfo.FreeGems = sc.FreeGems;
            user.teamInfo.PaidGems = sc.PaidGems;
            user.teamInfo.SummonTicket = sc.SummonTickets;
            user.teamInfo.ColosseumTicket = sc.ColosseumTickets;

            // Insert default userinfo to simulate proper login flow
            GME_DB->execSqlAsync(
                "INSERT INTO userinfo (id, level, exp, max_unit_count, max_friend_count, zel, karma, brave_coin, max_warehouse_count, want_gift, free_gems, paid_gems, active_deck, summon_tickets, rainbow_coins, colosseum_tickets, active_arena_deck, total_brave_points, avail_brave_points, energy) "
                "VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10, $11, $12, $13, $14, $15, $16, $17, $18, $19, $20)",
                [](const drogon::orm::Result& result) {
                LOG_INFO << "UserInfoHandler: Inserted default userinfo for new user";
            },
                [](const drogon::orm::DrogonDbException& e) {
                LOG_ERROR << "UserInfoHandler: Failed to insert default userinfo: " << e.base().what();
            },
                user.info.userID, user.teamInfo.Level, user.teamInfo.Exp, user.teamInfo.MaxUnitCount, user.teamInfo.MaxFriendCount,
                user.teamInfo.Zel, user.teamInfo.Karma, user.teamInfo.BraveCoin, user.teamInfo.WarehouseCount, user.teamInfo.WantGift,
                user.teamInfo.FreeGems, user.teamInfo.PaidGems, user.teamInfo.ActiveDeck, user.teamInfo.SummonTicket, user.teamInfo.RainbowCoin,
                user.teamInfo.ColosseumTicket, user.teamInfo.ArenaDeckNum, user.teamInfo.BravePointsTotal, user.teamInfo.CurrentBravePoints, user.teamInfo.ActionPoint
            );
        }

        const auto& msts = System::Instance().MstConfig();
        {
            const auto& p = msts.GetProgressionInfo().Mst.at(user.teamInfo.Level - 1);
            user.teamInfo.DeckCost = p.deckCost;
            user.teamInfo.MaxFriendCount = p.friendCount;
            user.teamInfo.AddFriendCount = p.addFriendCount;
            user.teamInfo.MaxActionPoint = p.actionPoints;
            if (user.teamInfo.ActionPoint == 0)
                user.teamInfo.ActionPoint = user.teamInfo.MaxActionPoint;
        }

        GME_DB->execSqlAsync(
            "SELECT unit_id, name, rarity, element, stats, bb, leader_skill, ai FROM user_units WHERE user_id = $1 LIMIT 500", // LIMIT 1470 here at end
            [this, &user, cb](const drogon::orm::Result& unitResult) {
            LOG_INFO << "UserInfoHandler: Found " << unitResult.size() << " units for user_id: " << user.info.userID;

            Json::Value res;
            user.info.Serialize(res);
            user.teamInfo.Serialize(res);

            {
                Response::UserLoginCampaignInfo v;
                v.currentDay = 1;
                v.totalDays = 96;
                v.firstForTheDay = true;
                v.Serialize(res);
            }

            {
                Response::UserTeamArchive v;
                v.Serialize(res);
            }

            {
                Response::UserTeamArenaArchive v;
                v.Serialize(res);
            }

            Response::UserUnitInfo unitInfo;
            for (const auto& row : unitResult) {
                Response::UserUnitInfo::Data d;
                d.userID = user.info.userID;
                d.userUnitID = std::stoul(row["unit_id"].as<std::string>());
                d.unitID = std::stoul(row["unit_id"].as<std::string>());
                d.unitTypeID = row["rarity"].as<uint32_t>();
                d.element = row["element"].as<std::string>();
                d.unitLv = 1;
                d.newFlg = 1;
                d.receiveDate = 100;
                d.FeBP = 100;
                d.FeMaxUsableBP = 200;

                Json::Value stats;
                Json::Reader reader;
                std::string statsStr = row["stats"].as<std::string>();
                if (!reader.parse(statsStr, stats)) {
                    LOG_ERROR << "Failed to parse stats JSON: " << statsStr;
                    stats = Json::Value();
                }
                d.baseHp = stats.isMember("hp") ? stats["hp"].asUInt() : 1000;
                d.baseAtk = stats.isMember("atk") ? stats["atk"].asUInt() : 1000;
                d.baseDef = stats.isMember("def") ? stats["def"].asUInt() : 1000;
                d.baseHeal = stats.isMember("rec") ? stats["rec"].asUInt() : 1000;
                d.addHp = d.baseHp / 10;
                d.addAtk = d.baseAtk / 10;
                d.addDef = d.baseDef / 10;
                d.addHeal = d.baseHeal / 10;
                d.extHp = d.addHp;
                d.extAtk = d.addAtk;
                d.extDef = d.addDef;
                d.extHeal = d.addHeal;
                d.limitOverHP = d.baseHp / 5;
                d.limitOverAtk = d.baseAtk / 5;
                d.limitOverDef = d.baseDef / 5;
                d.limitOverHeal = d.baseHeal / 5;
                d.exp = 1;
                d.totalExp = 1;

                unitInfo.Mst.emplace_back(d);
            }

            // Fallback: Add a dummy unit if inventory is empty
            if (unitInfo.Mst.empty()) {
                LOG_WARN << "UserInfoHandler: No units found, adding dummy unit for compatibility";
                Response::UserUnitInfo::Data d;
                d.userID = user.info.userID;
                d.userUnitID = 9999;
                d.unitID = 9999;
                d.unitTypeID = 1;
                d.element = "fire";
                d.unitLv = 1;
                d.newFlg = 1;
                d.receiveDate = 100;
                d.FeBP = 100;
                d.FeMaxUsableBP = 200;
                d.baseHp = 1000;
                d.baseAtk = 1000;
                d.baseDef = 1000;
                d.baseHeal = 1000;
                d.addHp = 100;
                d.addAtk = 100;
                d.addDef = 100;
                d.addHeal = 100;
                d.extHp = 100;
                d.extAtk = 100;
                d.extDef = 100;
                d.extHeal = 100;
                d.limitOverHP = 200;
                d.limitOverAtk = 200;
                d.limitOverDef = 200;
                d.limitOverHeal = 200;
                d.exp = 1;
                d.totalExp = 1;
                unitInfo.Mst.emplace_back(d);
            }

            unitInfo.Serialize(res);

            {
                Response::UserPartyDeckInfo deckInfo;
                if (!unitInfo.Mst.empty()) {
                    Response::UserPartyDeckInfo::Data d;
                    d.deckNum = 0;
                    d.deckType = 1;
                    d.dispOrder = 0;
                    d.memberType = 0; // Leader
                    d.userUnitID = unitInfo.Mst[0].userUnitID;
                    deckInfo.Mst.emplace_back(d);
                }
                deckInfo.Serialize(res);
            }

            {
                Response::UserUnitDictionary v;
                v.Serialize(res);
            }

            {
                Response::UserFavorite v;
                v.Serialize(res);
            }

            {
                Response::UserClearMissionInfo v;
                v.Serialize(res);
            }

            {
                Response::UserWarehouseInfo v;
                v.Serialize(res);
            }

            {
                Response::ItemFavorite v;
                v.Serialize(res);
            }

            {
                Response::UserItemDictionaryInfo v;
                v.Serialize(res);
            }

            {
                Response::UserArenaInfo v;
                v.Serialize(res);
            }

            {
                Response::UserGiftInfo v;
                v.Serialize(res);
            }

            {
                Response::SummonerJournalUserInfo v;
                v.userId = user.info.userID;
                v.Serialize(res);
            }

            {
                Response::SignalKey v;
                v.key = "5EdKHavF";
                v.Serialize(res);
            }

            System::Instance().MstConfig().CopyUserInfoMstTo(res);

            cb(newGmeOkResponse(GetGroupId(), GetAesKey(), res));
        },
            [this, cb](const drogon::orm::DrogonDbException& e) { OnError(e, cb); },
            user.info.userID
        );
    },
        [this, cb](const drogon::orm::DrogonDbException& e) { OnError(e, cb); },
        user.info.userID
    );
}