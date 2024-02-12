#include "ServerConfig.hpp"

bool ServerConfig::ParseFromJson(const Json::Value& v)
{
	try
	{
		Wallpaper = v["wallpaper_banner"].asCString();
		GameVersion = v["game_version"].asInt();

		Json::Value si = v["start_info"];

		Start.UnitCount = si["unit_count"].asUInt();
		Start.ItemCount = si["item_count"].asUInt();
		Start.FreeGems = si["free_gems"].asUInt();
		Start.SummonTickets = si["summon_tickets"].asUInt();
		Start.PaidGems = si["paid_gems"].asUInt();
		Start.Zel = si["zel"].asUInt64();
		Start.Karma = si["karma"].asUInt64();
		Start.ColosseumTickets = si["colosseum_tickets"].asUInt();
		Start.Level = si["level"].asUInt();
		Start.FriendCount = si["friend_count"].asUInt();
	}
	catch (std::exception& ex)
	{
		return false;
	}

	return true;
}

