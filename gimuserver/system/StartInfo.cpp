#include "StartInfo.hpp"
#include <fstream>
#include <json/json.h>

void StartInfo::LoadTableFromJson(const std::string& path)
{
	std::ifstream ifs;
	ifs.open(path + "/start_info.json");

	if (!ifs.is_open())
		throw std::runtime_error("start_info.json: not found");

	Json::CharReaderBuilder rb;
	JSONCPP_STRING jss;
	Json::Value root;
	if (!Json::parseFromStream(rb, ifs, &root, &jss))
		throw std::runtime_error("start_info.json: parse error");

	UnitCount = root["unit_count"].asUInt();
	ItemCount = root["item_count"].asUInt();
	FreeGems = root["free_gems"].asUInt();
	SummonTickets = root["summon_tickets"].asUInt();
	PaidGems = root["paid_gems"].asUInt();
	Zel = root["zel"].asUInt64();
	Karma = root["karma"].asUInt64();
	ColosseumTickets = root["colosseum_tickets"].asUInt();
	Level = root["level"].asUInt();
	FriendCount = root["friend_count"].asUInt();
}
