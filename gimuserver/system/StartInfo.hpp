#pragma once

#include <string>

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

	explicit StartInfo() : UnitCount(0), ItemCount(0),
		FreeGems(0), SummonTickets(0), PaidGems(0),
		Zel(0), Karma(0), ColosseumTickets(0),
		Level(0), FriendCount(0) {}

	void LoadTableFromJson(const std::string& path);
};
