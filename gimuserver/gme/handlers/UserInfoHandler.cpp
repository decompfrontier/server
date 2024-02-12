#include "UserInfoHandler.hpp"
#include <db/DbMacro.hpp>
#include <core/Utils.hpp>
#include <core/System.hpp>
#include <gme/response/UserTeamInfo.hpp>

void Handler::UserInfoHandler::Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const
{
	GME_DB->execSqlAsync(
		"SELECT level, exp, max_unit_count, max_friend_count, "
		"zel, karma, brave_coin, max_warehouse_count, want_gift, "
		"free_gems, paid_gems, active_deck, summon_tickets, "
		"rainbow_coins, colosseum_tickets, active_arena_deck, "
		"total_brave_points, avail_brave_points "
		"FROM userinfo WHERE id = $1", [this, &user, cb](const drogon::orm::Result& result) {

			if (result.size() > 0)
			{
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
			}
			else
			{
				auto sc = System::Instance().ServerConfig().Start;
				user.teamInfo.UserID = user.info.userID;
				user.teamInfo.Level = sc.Level;
				user.teamInfo.Exp = 0;
				user.teamInfo.MaxUnitCount = sc.UnitCount;
				user.teamInfo.MaxFriendCount = sc.FriendCount;
				user.teamInfo.Zel = sc.Zel;
				user.teamInfo.Karma = sc.Karma;
				user.teamInfo.BraveCoin = 0;
				user.teamInfo.WarehouseCount = sc.UnitCount;
				user.teamInfo.FreeGems = sc.FreeGems;
				user.teamInfo.PaidGems = sc.PaidGems;
				user.teamInfo.SummonTicket = sc.SummonTickets;
				user.teamInfo.ColosseumTicket = sc.ColosseumTickets;
			}

			Json::Value v;
			user.teamInfo.Serialize(v);
			cb(newGmeOkResponse(GetGroupId(), GetAesKey(), v));
		}, 
		[this, cb](const drogon::orm::DrogonDbException& e) { OnError(e, cb); },
		user.info.userID
	);
}
