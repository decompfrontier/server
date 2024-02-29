#pragma once

#include "../GmeRequest.hpp"

RESPONSE_NS_BEGIN
struct UserTeamInfo : public IResponse
{
	const char* getGroupName() const override { return "fEi17cnx"; }

	UserTeamInfo() : Level(0), Exp(0), MaxActionPoint(0), ActionPoint(0),
		MaxFightPoint(0), FightPoint(0), MaxUnitCount(0), AddUnitCount(0),
		DeckCost(0), MaxEquipSlotCount(0), MaxFriendCount(0), AddFriendCount(0),
		FriendPoint(0), Zel(0), Karma(0), BraveCoin(0), WarehouseCount(0),
		AddWarehouseCount(0), PresentCount(0), FriendAgreeCount(0),
		FightRestTimer(0), FreeGems(0), SummonTicket(0),
		SlotGameFlag(0), RainbowCoin(0), BravePointsTotal(0),
		ColosseumTicket(0), ArenaDeckNum(0), ReinforcementDeck(0),
		ReinforcementDeckEx1(0), ReinforcementDeckEx2(0),
		PaidGems(0), ActionRestTimer(0), ActiveDeck(0), GiftReceiveCount(0),
		MysteryBoxCount(0), CompletedTaskCount(0), InboxMessageCount(0),
		CurrentBravePoints(0)
	{}

	std::string UserID;
	uint32_t Level;
	uint64_t Exp;
	uint32_t MaxActionPoint; // ?
	uint32_t ActionPoint; // ?
	uint32_t MaxFightPoint; // ?
	uint32_t FightPoint; // ?
	uint32_t MaxUnitCount;
	uint32_t AddUnitCount; // ?
	uint32_t DeckCost;
	uint32_t MaxEquipSlotCount; // ?
	uint32_t MaxFriendCount;
	uint32_t AddFriendCount; // ?
	uint32_t FriendPoint; // ?
	uint64_t Zel;
	uint64_t Karma;
	uint32_t BraveCoin;
	std::string FriendMessage; // ?
	uint32_t WarehouseCount;
	uint32_t AddWarehouseCount; // ?
	std::string WantGift;
	uint32_t PresentCount; // ?
	uint32_t FriendAgreeCount; // ?
	uint32_t GiftReceiveCount; // ?
	uint32_t ActionRestTimer; // ?
	uint32_t FightRestTimer; // ?
	uint32_t FreeGems;
	uint32_t ActiveDeck;
	uint32_t SummonTicket;
	uint32_t SlotGameFlag; // ?
	uint32_t RainbowCoin; // ?
	uint32_t BravePointsTotal;
	uint32_t ColosseumTicket;
	uint32_t ArenaDeckNum;
	uint32_t ReinforcementDeck; // ?
	uint32_t ReinforcementDeckEx1; // ?
	uint32_t ReinforcementDeckEx2; // ?
	uint32_t PaidGems;
	uint32_t MysteryBoxCount; // ?
	uint32_t CompletedTaskCount; // ?
	uint32_t InboxMessageCount; // ?
	uint32_t CurrentBravePoints;

protected:
	void SerializeFields(Json::Value& v, size_t) const override
	{
		v["h7eY3sAK"] = UserID;
		v["D9wXQI2V"] = std::to_string(Level);
		v["d96tuT2E"] = std::to_string(Exp);
		v["YnM14RIP"] = std::to_string(MaxActionPoint);
		v["0P9X1YHs"] = std::to_string(ActionPoint);
		v["9m5FWR8q"] = std::to_string(MaxFightPoint);
		v["YS2JG9no"] = std::to_string(FightPoint);
		v["ouXxIY63"] = std::to_string(MaxUnitCount);
		v["Px1X7fcd"] = std::to_string(AddUnitCount);
		v["QYP4kId9"] = std::to_string(DeckCost);
		v["gEX30r1b"] = std::to_string(MaxEquipSlotCount);
		v["3u41PhR2"] = std::to_string(MaxFriendCount);
		v["2rR5s6wn"] = std::to_string(AddFriendCount);
		v["J3stQ7jd"] = std::to_string(FriendPoint);
		v["Najhr8m6"] = std::to_string(Zel);
		v["HTVh8a65"] = std::to_string(Karma);
		v["03UGMHxF"] = BraveCoin;
		v["bM7RLu5K"] = FriendMessage;
		v["5pjoGBC4"] = std::to_string(WarehouseCount);
		v["iI7Wj6pM"] = std::to_string(AddWarehouseCount);
		v["s2WnRw9N"] = WantGift;
		v["EfinBo65"] = std::to_string(PresentCount);
		v["qVBx7g2c"] = std::to_string(FriendAgreeCount);
		v["1RQT92uE"] = std::to_string(GiftReceiveCount);
		v["f0IY4nj8"] = std::to_string(ActionRestTimer);
		v["jp9s8IyY"] = std::to_string(FightRestTimer);
		v["92uj7oXB"] = FreeGems;
		v["Z0Y4RoD7"] = std::to_string(ActiveDeck);
		v["9r3aLmaB"] = std::to_string(SummonTicket);
		v["s3uU4Lgb"] = SlotGameFlag;
		v["KAZmxkgy"] = std::to_string(RainbowCoin);
		v["bya9a67k"] = std::to_string(BravePointsTotal);
		v["lKuj3Ier"] = std::to_string(ColosseumTicket);
		v["gKNfIZiA"] = ArenaDeckNum;
		v["TwqMChon"] = std::to_string(ReinforcementDeck) + "," + std::to_string(ReinforcementDeckEx1) + "," + std::to_string(ReinforcementDeckEx2);
		v["d37CaiX1"] = PaidGems;
		v["Qo9doUsp"] = MysteryBoxCount;
		v["7qncTHUJ"] = InboxMessageCount;
		v["3a8b9D8i"] = std::to_string(CompletedTaskCount);
		v["22rqpZTo"] = std::to_string(CurrentBravePoints);
	}
};
RESPONSE_NS_END
