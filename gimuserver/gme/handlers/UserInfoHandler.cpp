#include "UserInfoHandler.hpp"
#include <db/DbMacro.hpp>
#include <core/Utils.hpp>
#include <core/System.hpp>
#include <gme/response/UserTeamInfo.hpp>

void Handler::UserInfoHandler::Handle(const Json::Value& req)
{
	auto& p = GME_DB;
	p->execSqlAsync(
		"SELECT id, level, exp, max_unit_count, max_friend_count, "
		"zel, karma, brave_coin, max_warehouse_count, want_gift, "
		"free_gems, paid_gems, active_deck, summon_tickets, "
		"rainbow_coins, colosseum_tickets, active_arena_deck, "
		"total_brave_points, avail_brave_points"
		"FROM userinfo", [this](const drogon::orm::Result& result) {
			Response::UserTeamInfo ti;

			if (result.size() > 0)
			{
				int col = 0;
				auto& sql = result[0];
				ti.UserID = sql[col++].as<std::string>();
				ti.Level = sql[col++].as<uint32_t>();
				ti.Exp = sql[col++].as<int64_t>();
				ti.MaxUnitCount = sql[col++].as<uint32_t>();
				ti.MaxFriendCount = sql[col++].as<uint32_t>();
				ti.Zel = sql[col++].as<uint64_t>();
				ti.Karma = sql[col++].as<uint64_t>();
				ti.BraveCoin = sql[col++].as<uint32_t>();
				ti.WarehouseCount = sql[col++].as<uint32_t>();
				ti.WantGift = sql[col++].as<std::string>();
				ti.FreeGems = sql[col++].as<uint32_t>();
				ti.PaidGems = sql[col++].as<uint32_t>();
				ti.ActiveDeck = sql[col++].as<uint32_t>();
				ti.SummonTicket = sql[col++].as<uint32_t>();
				ti.RainbowCoin = sql[col++].as<uint32_t>();
				ti.ColosseumTicket = sql[col++].as<uint32_t>();
				ti.ArenaDeckNum = sql[col++].as<uint32_t>();
				ti.BravePointsTotal = sql[col++].as<uint32_t>();
				ti.CurrentBravePoints = sql[col++].as<uint32_t>();
			}
			else
			{
				auto sc = System::Instance().ServerConfig().Start;
				ti.UserID = Utils::RandomUserID();
				ti.Level = sc.Level;
				ti.Exp = 0;
				ti.MaxUnitCount = sc.UnitCount;
				ti.MaxFriendCount = sc.FriendCount;
				ti.Zel = sc.Zel;
				ti.Karma = sc.Karma;
				ti.BraveCoin = 0;
				ti.WarehouseCount = sc.UnitCount;
				ti.FreeGems = sc.FreeGems;
				ti.PaidGems = sc.PaidGems;
				ti.SummonTicket = sc.SummonTickets;
				ti.ColosseumTicket = sc.ColosseumTickets;
			}

			Json::Value v;
			ti.Serialize(v);
			FinishHandling(v);
		}, 
		[this](const drogon::orm::DrogonDbException& e) {
			m_errMsg = e.base().what();
			m_errID = ErrorID::Yes;
			FinishHandling();
	});
}
