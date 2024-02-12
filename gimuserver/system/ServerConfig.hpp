#pragma once

#include <json/value.h>

struct ServerConfig
{
	struct StartInfo
	{
		uint32_t UnitCount;
		uint32_t ItemCount;
		uint32_t FreeGems;
		uint32_t SummonTickets;
		uint32_t PaidGems;
		uint64_t Zel;
		uint64_t Karma;
		uint32_t ColosseumTickets;
		uint32_t Level;
		uint32_t FriendCount;
	} Start;

	std::string Wallpaper;
	int GameVersion;

	void ParseFromJson(const Json::Value& v);
};
