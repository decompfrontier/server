#include "GetUserInfoRequestHandler.hpp"
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

namespace {
    // Helper: Populate unit data structure with default values
    void PopulateUnitData(Response::UserUnitInfo::Data& d, const std::string& userId,
        uint32_t userUnitId, uint64_t unitId) {
        d.userID = userId;
        d.userUnitID = userUnitId;
        d.unitID = unitId;
        d.element = "fire";
        d.unitLv = 1;
        d.newFlg = 1;
        d.receiveDate = 100;
        d.FeBP = 100;
        d.FeMaxUsableBP = 200;

        // Base stats - all start at 1000
        d.baseHp = d.baseAtk = d.baseDef = d.baseHeal = 1000;
        d.addHp = d.addAtk = d.addDef = d.addHeal = 100;
        d.extHp = d.extAtk = d.extDef = d.extHeal = 100;
        d.limitOverHP = d.limitOverAtk = d.limitOverDef = d.limitOverHeal = 200;
        d.exp = d.totalExp = 1;

        // Default skill and equipment IDs
        d.unitTypeID = 1;
        d.leaderSkillID = d.skillID = d.extraSkillID = 0;
        d.eqipItemFrameID = d.eqipItemFrameID2 = 0;
        d.eqipItemID = d.equipItemID2 = 0;
        d.ExtraPassiveSkillID = d.ExtraPassiveSkillID2 = d.AddExtraPassiveSkillID = 0;
        d.FeSkillInfo = "";
    }

    // Helper: Create dummy unit for empty inventory (Vargas starter unit)
    Response::UserUnitInfo::Data CreateDummyUnit(const std::string& userId) {
        Response::UserUnitInfo::Data d;
        PopulateUnitData(d, userId, 1, 10017); // Unit ID 10017 = Vargas
        return d;
    }

    // Helper: Build unit info from database result
    Response::UserUnitInfo BuildUnitInfo(const drogon::orm::Result& unitResult, const std::string& userId) {
        Response::UserUnitInfo unitInfo;

        for (const auto& row : unitResult) {
            Response::UserUnitInfo::Data d;
            // unit_id may have suffixes like _100 (limit broken) or _2 (evolved form).
            // Strip everything after the first underscore to get the base numeric ID.
            std::string rawId = row["unit_id"].as<std::string>();
            auto underscore = rawId.find('_');
            uint64_t unitId = std::stoull(
                underscore == std::string::npos ? rawId : rawId.substr(0, underscore)
            );
            PopulateUnitData(d, userId, row["id"].as<uint32_t>(), unitId);
            unitInfo.Mst.emplace_back(d);
        }

        // Add dummy unit if inventory is empty to prevent client crashes
        if (unitInfo.Mst.empty()) {
            LOG_WARN << "No units found for user " << userId << ", adding dummy unit";
            unitInfo.Mst.emplace_back(CreateDummyUnit(userId));
        }

        return unitInfo;
    }

    // Helper: Create party deck with first unit as leader
    Response::UserPartyDeckInfo CreateDefaultDeck(const Response::UserUnitInfo& unitInfo) {
        Response::UserPartyDeckInfo deckInfo;

        if (!unitInfo.Mst.empty()) {
            Response::UserPartyDeckInfo::Data d;
            d.deckNum = 0;
            d.deckType = 1;
            d.dispOrder = 0;
            d.memberType = 0; // Leader position
            d.userUnitID = unitInfo.Mst[0].userUnitID;
            deckInfo.Mst.emplace_back(d);
        }

        return deckInfo;
    }
}

void Handler::GetUserInfoRequestHandler::Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const {
    // The Windows client sends dummy placeholder values like '0101AABB' as user_id
    // inside the encrypted GME packet body. The GmeController copies that into
    // user.info.userID before calling us, so we cannot trust it directly.
    //
    // Resolution: query the users table. If the current userID exists, use it.
    // Otherwise (UNION ALL) fall back to the first real user in the DB.
    // For offline single-player this always resolves to the one real account.
    LOG_INFO << "UserInfoHandler: raw user_id: " << user.info.userID;

    GME_DB->execSqlAsync(
        "SELECT id FROM users WHERE id = $1 "
        "UNION ALL "
        "SELECT id FROM users WHERE id != $1 LIMIT 1",
        [this, &user, cb, req](const drogon::orm::Result& idResult)
        {
            if (!idResult.empty())
            {
                std::string resolvedId = idResult[0]["id"].as<std::string>();
                if (resolvedId != user.info.userID)
                {
                    LOG_INFO << "UserInfoHandler: remapped " << user.info.userID
                        << " -> " << resolvedId;
                    user.info.userID = resolvedId;
                }
            }
            else
            {
                LOG_WARN << "UserInfoHandler: no users in DB";
            }
            // Now dispatch with the resolved user ID.
            HandleResolved(user, cb, req);
        },
        [this, &user, cb, req](const drogon::orm::DrogonDbException& e)
        {
            LOG_ERROR << "UserInfoHandler: resolution query failed: " << e.base().what();
            HandleResolved(user, cb, req);
        },
        user.info.userID
    );
}

void Handler::GetUserInfoRequestHandler::HandleResolved(UserInfo& user, DrogonCallback cb, const Json::Value& req) const {
    LOG_INFO << "UserInfoHandler: resolved user_id: " << user.info.userID;

    // Handle unit inventory or squad management requests separately
    std::string action = req.isMember("action") ? req["action"].asString() : "";
    if (action == "load_unit_inventory" || action == "Zw3WIoWu" || action == "load_squad_management") {
        GME_DB->execSqlAsync(
            "SELECT id, unit_id FROM user_units WHERE user_id = $1 LIMIT 4000",
            [this, &user, cb](const drogon::orm::Result& unitResult) {
                LOG_INFO << "Found " << unitResult.size() << " units for user " << user.info.userID;

                Json::Value res;
                auto unitInfo = BuildUnitInfo(unitResult, user.info.userID);
                unitInfo.Serialize(res);

                auto deckInfo = CreateDefaultDeck(unitInfo);
                deckInfo.Serialize(res);

                cb(newGmeOkResponse(GetGroupId(), GetAesKey(), res));
            },
            [this, cb](const drogon::orm::DrogonDbException& e) { OnError(e, cb); },
            user.info.userID
        );
        return;
    }

    // Main user info loading logic
    GME_DB->execSqlAsync(
        "SELECT level, exp, max_unit_count, max_friend_count, zel, karma, brave_coin, "
        "max_warehouse_count, want_gift, free_gems, paid_gems, active_deck, summon_tickets, "
        "rainbow_coins, colosseum_tickets, active_arena_deck, total_brave_points, "
        "avail_brave_points, energy FROM userinfo WHERE id = $1",
        [this, &user, cb](const drogon::orm::Result& result) {
            // Load existing user data or initialize defaults
            if (result.size() > 0) {
                auto& sql = result[0];
                int col = 0;
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
                // Initialize new user with starter config
                //TODO: Call New User Logic Here
                auto sc = System::Instance().MstConfig().StartInfo();
                user.teamInfo.UserID = user.info.userID;
                user.teamInfo.Level = sc.Level;
                user.teamInfo.Exp = 0;
                user.teamInfo.MaxUnitCount = user.teamInfo.WarehouseCount = 4000;
                user.teamInfo.MaxFriendCount = sc.FriendCount;
                user.teamInfo.Zel = sc.Zel;
                user.teamInfo.Karma = sc.Karma;
                user.teamInfo.BraveCoin = 0;
                user.teamInfo.FreeGems = sc.FreeGems;
                user.teamInfo.PaidGems = sc.PaidGems;
                user.teamInfo.SummonTicket = sc.SummonTickets;
                user.teamInfo.ColosseumTicket = sc.ColosseumTickets;

                // Persist new user to database
                GME_DB->execSqlAsync(
                    "INSERT INTO userinfo (id, level, exp, max_unit_count, max_friend_count, "
                    "zel, karma, brave_coin, max_warehouse_count, want_gift, free_gems, paid_gems, "
                    "active_deck, summon_tickets, rainbow_coins, colosseum_tickets, active_arena_deck, "
                    "total_brave_points, avail_brave_points, energy) "
                    "VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10, $11, $12, $13, $14, $15, $16, $17, $18, $19, $20)",
                    [](const drogon::orm::Result&) { LOG_INFO << "Inserted default userinfo for new user"; },
                    [](const drogon::orm::DrogonDbException& e) {
                        LOG_ERROR << "Failed to insert userinfo: " << e.base().what();
                    },
                    user.info.userID, user.teamInfo.Level, user.teamInfo.Exp, user.teamInfo.MaxUnitCount,
                    user.teamInfo.MaxFriendCount, user.teamInfo.Zel, user.teamInfo.Karma, user.teamInfo.BraveCoin,
                    user.teamInfo.WarehouseCount, user.teamInfo.WantGift, user.teamInfo.FreeGems, user.teamInfo.PaidGems,
                    user.teamInfo.ActiveDeck, user.teamInfo.SummonTicket, user.teamInfo.RainbowCoin,
                    user.teamInfo.ColosseumTicket, user.teamInfo.ArenaDeckNum, user.teamInfo.BravePointsTotal,
                    user.teamInfo.CurrentBravePoints, user.teamInfo.ActionPoint
                );
            }

            // Apply level-based progression modifiers
            const auto& msts = System::Instance().MstConfig();
            const auto& p = msts.GetProgressionInfo().Mst.at(user.teamInfo.Level - 1);
            user.teamInfo.DeckCost = p.deckCost;
            user.teamInfo.MaxFriendCount = p.friendCount;
            user.teamInfo.AddFriendCount = p.addFriendCount;
            user.teamInfo.MaxActionPoint = p.actionPoints;
            if (user.teamInfo.ActionPoint == 0)
                user.teamInfo.ActionPoint = user.teamInfo.MaxActionPoint;

            // Load user units and build complete response
            GME_DB->execSqlAsync(
                "SELECT id, unit_id FROM user_units WHERE user_id = $1 LIMIT 4000",
                [this, &user, cb](const drogon::orm::Result& unitResult) {
                    LOG_INFO << "Found " << unitResult.size() << " units for user " << user.info.userID;

                    // Build comprehensive response with all user data
                    Json::Value res;
                    user.info.Serialize(res);
                    user.teamInfo.Serialize(res);

                    // Add login campaign info
                    Response::UserLoginCampaignInfo campaign;
                    campaign.currentDay = 1;
                    campaign.totalDays = 96;
                    campaign.firstForTheDay = true;
                    campaign.Serialize(res);

                    // Add unit inventory
                    auto unitInfo = BuildUnitInfo(unitResult, user.info.userID);
                    unitInfo.Serialize(res);

                    // Add party deck
                    auto deckInfo = CreateDefaultDeck(unitInfo);
                    deckInfo.Serialize(res);

                    // Serialize all other game data structures
                    Response::UserTeamArchive{}.Serialize(res);
                    Response::UserTeamArenaArchive{}.Serialize(res);
                    Response::UserUnitDictionary{}.Serialize(res);
                    Response::UserFavorite{}.Serialize(res);
                    Response::UserClearMissionInfo{}.Serialize(res);
                    Response::UserWarehouseInfo{}.Serialize(res);
                    Response::ItemFavorite{}.Serialize(res);
                    Response::UserItemDictionaryInfo{}.Serialize(res);
                    Response::UserArenaInfo{}.Serialize(res);
                    Response::UserGiftInfo{}.Serialize(res);

                    // Add summoner journal info
                    Response::SummonerJournalUserInfo journal;
                    journal.userId = user.info.userID;
                    journal.Serialize(res);

                    // Add signal key for client authentication
                    Response::SignalKey signalKey;
                    signalKey.key = "5EdKHavF";
                    signalKey.Serialize(res);

                    // Append master config data
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