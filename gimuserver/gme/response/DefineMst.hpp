#pragma once

#include "../GmeRequest.hpp"

RESPONSE_NS_BEGIN
struct DefineMst : public IResponse
{
	const char* getGroupName() const override { return "VkoZ5t3K"; }
	bool isArray() const override { return false; }

	explicit DefineMst() : 
		maxZel(0), maxKarma(0), maxFriendPoint(0), maxSummonerFriendPoint(0), maxSummonerSP(0),
		friendPointReinFriendRate(0.0f), friendPointReinFriendMyRate(0.0f), friendPointReinOtherRate(0.0f),
		friendPointReinOtherMyRate(0.0f), unitMixSuperExpRate(0.0f), unitMixGreatExpRate(0.0f),
		maxTeamLv(0), maxArenaRank(0), 
		maxUnitCount(0), frinedPointReinFriend(0), friendPointReinFriendMy(0),
		maxWarehouseCount(0), defaultUnitCount(0), friendPointReinOther(0),
		defaultWarehouseCount(0), friendPointReinOtherMy(0), permitInvitation(0),
		extPlusMaxCount(0), actionPointHealCount(0), fightPointHealCount(0),
		unitBoxExtCount(0), continueDiaCount(0), itemBoxExtCount(0), tutorialDungeonID(0),
		recoverTimeAction(0), verifyFlag(0), arenaBattleTimeLimit(0), recoverTimeFight(0),
		arenaNeedMissionID(0), maxPartyDeckCount(0), recoverTimeFrohun(0), recoverTimeRaid(0),
		raidMaxP(0), raidBagCount(0), friendExtCount(0), medalMaxNum(0),
		maxAchievePoint(0), zelPerAchievePoint(0), karmaPerAchievePoint(0), maxAchievePointZelPerDay(0),
		maxAchievePointKarmaPerDay(0), maxAchievePointItemPerDay(0), maxAchievementChallengeAcceptCount(0),
		maxAchievementRecordChallengeAcceptCount(0), tutorialGatchaID(0),
		colosseumShopTicket(0), maxColosseumTicket(0), maxCBP(0), actionPointThreshold(0),
		resetFeSkillDiaCount(0), maxBlackListCount(0), maxMultiP(0), actionPointRecoverFixed(0)
	{}

	uint64_t maxZel, maxKarma, maxFriendPoint, maxSummonerFriendPoint, maxSummonerSP;
	float friendPointReinFriendRate, friendPointReinFriendMyRate, friendPointReinOtherRate,
		friendPointReinOtherMyRate, unitMixSuperExpRate, unitMixGreatExpRate;
	uint32_t maxTeamLv, maxArenaRank,
		maxUnitCount, frinedPointReinFriend, friendPointReinFriendMy,
		maxWarehouseCount, defaultUnitCount, friendPointReinOther,
		defaultWarehouseCount, friendPointReinOtherMy, permitInvitation,
		extPlusMaxCount, actionPointHealCount, fightPointHealCount,
		unitBoxExtCount, continueDiaCount, itemBoxExtCount, tutorialDungeonID,
		recoverTimeAction, verifyFlag, arenaBattleTimeLimit, recoverTimeFight,
		arenaNeedMissionID, maxPartyDeckCount, recoverTimeFrohun, recoverTimeRaid,
		raidMaxP, raidBagCount, friendExtCount, medalMaxNum,
		maxAchievePoint, zelPerAchievePoint, karmaPerAchievePoint, maxAchievePointZelPerDay,
		maxAchievePointKarmaPerDay, maxAchievePointItemPerDay, maxAchievementChallengeAcceptCount,
		maxAchievementRecordChallengeAcceptCount, tutorialGatchaID,
		colosseumShopTicket, maxColosseumTicket, maxCBP, actionPointThreshold,
		resetFeSkillDiaCount, maxBlackListCount, maxMultiP, actionPointRecoverFixed;
	std::string dbbCrystalValues, dbbFixedSettingsValues, initialUnit, arenaTutorialNpcInfo,
		parseOverDriveParam, initSummonerArmID, campaignFlag;

protected:
	void SerializeFields(Json::Value& v, size_t) const override
	{
		v["DXm2W8vY"] = std::to_string(maxZel);
		v["jSxdlwqd"] = std::to_string(maxKarma);
		v["4YCzox9t"] = std::to_string(maxFriendPoint);
		v["Kt8H4LN7"] = std::to_string(maxTeamLv);
		v["tzCfGX83"] = std::to_string(maxArenaRank);
		v["ouXxIY63"] = std::to_string(maxUnitCount);
		v["5pjoGBC4"] = std::to_string(maxWarehouseCount);
		v["k0xrd38b"] = std::to_string(defaultUnitCount);
		v["8U93zxsf"] = std::to_string(defaultWarehouseCount);
		v["2oD1fmuX"] = std::to_string(frinedPointReinFriend);
		v["PnE6xo93"] = std::to_string(friendPointReinFriendRate); // float
		v["86sx4FfJ"] = std::to_string(friendPointReinFriendMy);
		v["JBR0Po3b"] = std::to_string(friendPointReinFriendMyRate); // float
		v["K10QSeuj"] = std::to_string(friendPointReinOther);
		v["6p4YW7oc"] = std::to_string(friendPointReinOtherRate); // float
		v["VkA3nu0b"] = std::to_string(friendPointReinOtherMy);
		v["mI1jW0X7"] = std::to_string(friendPointReinOtherMyRate); // float
		v["02IgM6ib"] = std::to_string(permitInvitation);
		v["4t3qX2kT"] = std::to_string(extPlusMaxCount);
		v["C8KkHGa7"] = std::to_string(actionPointHealCount);
		v["sy9G24Su"] = std::to_string(fightPointHealCount);
		v["I73XkAQi"] = std::to_string(unitBoxExtCount);
		v["CA01vo2Q"] = std::to_string(itemBoxExtCount);
		v["QW3HiNv8"] = std::to_string(continueDiaCount);
		v["21ovwqYT"] = initialUnit; // str
		v["91nRcYWT"] = std::to_string(tutorialDungeonID);
		v["Ieq49JDy"] = std::to_string(recoverTimeAction);
		v["0BPn68DG"] = std::to_string(recoverTimeFight);
		v["YR4HI56k"] = std::to_string(arenaBattleTimeLimit);
		v["yFRYDj67"] = std::to_string(arenaNeedMissionID);
		v["WHy3BSm9"] = std::to_string(maxPartyDeckCount);
		v["6W4PdoJY"] = arenaTutorialNpcInfo; // str
		v["6GXx4LgZ"] = std::to_string(verifyFlag);
		v["2inP0tCg"] = std::to_string(unitMixGreatExpRate); // float
		v["zn65EXYF"] = std::to_string(unitMixSuperExpRate); // float
		v["3xAsgHL8"] = std::to_string(recoverTimeFrohun);
		v["zkD98Hfy"] = std::to_string(recoverTimeRaid);
		v["S92Hcor3"] = std::to_string(raidMaxP);
		v["09EbcDmX"] = std::to_string(raidBagCount);
		v["3Ep5akHJ"] = std::to_string(friendExtCount);
		v["xq0fSrw3"] = std::to_string(medalMaxNum);
		v["MFz8YRS6"] = campaignFlag;
		v["1JFcDr05"] = std::to_string(maxAchievePoint);
		v["SAb3m9wo"] = std::to_string(zelPerAchievePoint);
		v["KCG5f1AN"] = std::to_string(karmaPerAchievePoint);
		v["4ARtfF7x"] = std::to_string(maxAchievePointZelPerDay);
		v["K0sUIn8R"] = std::to_string(maxAchievePointKarmaPerDay);
		v["M1AJuFU6"] = std::to_string(maxAchievePointItemPerDay);
		v["p3pXbuHA"] = std::to_string(maxAchievementChallengeAcceptCount);
		v["kmxPgJu9"] = std::to_string(maxAchievementRecordChallengeAcceptCount);
		v["uALQnngx"] = std::to_string(tutorialGatchaID);
		v["QylsZTpE"] = parseOverDriveParam; // str
		v["OjAiNSoh"] = std::to_string(colosseumShopTicket);
		v["924iwrJ9"] = std::to_string(maxColosseumTicket);
		v["woxAcRoH"] = std::to_string(maxCBP);
		v["5csFoG1G"] = std::to_string(resetFeSkillDiaCount);
		v["DYMUxgt8"] = std::to_string(maxBlackListCount);
		v["deYOowYJ"] = initSummonerArmID; // str
		v["QhV0G2zu"] = std::to_string(maxSummonerSP); // u64
		v["W9bwut7Q"] = std::to_string(maxSummonerFriendPoint); // u64
		v["6id2v7eN"] = std::to_string(maxMultiP);
		//v["P_EGG_UNIT"] = std::to_string(pEggUnit);
		v["jFdW1ipx"] = dbbCrystalValues; // str
		v["7o6lcc66"] = dbbFixedSettingsValues; // str

		if (actionPointRecoverFixed > 0)
			v["hAiXsSPF"] = std::to_string(actionPointRecoverFixed);

		if (actionPointThreshold > 0)
			v["eRQvzLeF"] = std::to_string(actionPointThreshold);
	}
};
RESPONSE_NS_END
