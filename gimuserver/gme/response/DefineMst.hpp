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
		v["DXm2W8vY"] = maxZel;
		v["jSxdlwqd"] = maxKarma;
		v["4YCzox9t"] = maxFriendPoint;
		v["Kt8H4LN7"] = maxTeamLv;
		v["tzCfGX83"] = maxArenaRank;
		v["ouXxIY63"] = maxUnitCount;
		v["5pjoGBC4"] = maxWarehouseCount;
		v["k0xrd38b"] = defaultUnitCount;
		v["8U93zxsf"] = defaultWarehouseCount;
		v["2oD1fmuX"] = frinedPointReinFriend;
		v["PnE6xo93"] = friendPointReinFriendRate; // float
		v["86sx4FfJ"] = friendPointReinFriendMy;
		v["JBR0Po3b"] = friendPointReinFriendMyRate; // float
		v["K10QSeuj"] = friendPointReinOther;
		v["6p4YW7oc"] = friendPointReinOtherRate; // float
		v["VkA3nu0b"] = friendPointReinOtherMy;
		v["mI1jW0X7"] = friendPointReinOtherMyRate; // float
		v["02IgM6ib"] = permitInvitation;
		v["4t3qX2kT"] = extPlusMaxCount;
		v["C8KkHGa7"] = actionPointHealCount;
		v["sy9G24Su"] = fightPointHealCount;
		v["I73XkAQi"] = unitBoxExtCount;
		v["CA01vo2Q"] = itemBoxExtCount;
		v["QW3HiNv8"] = continueDiaCount;
		v["21ovwqYT"] = initialUnit; // str
		v["91nRcYWT"] = tutorialDungeonID;
		v["Ieq49JDy"] = recoverTimeAction;
		v["0BPn68DG"] = recoverTimeFight;
		v["YR4HI56k"] = arenaBattleTimeLimit;
		v["yFRYDj67"] = arenaNeedMissionID;
		v["WHy3BSm9"] = maxPartyDeckCount;
		v["6W4PdoJY"] = arenaTutorialNpcInfo; // str
		v["6GXx4LgZ"] = verifyFlag;
		v["2inP0tCg"] = unitMixGreatExpRate; // float
		v["zn65EXYF"] = unitMixSuperExpRate; // float
		v["3xAsgHL8"] = recoverTimeFrohun;
		v["zkD98Hfy"] = recoverTimeRaid;
		v["S92Hcor3"] = raidMaxP;
		v["09EbcDmX"] = raidBagCount;
		v["3Ep5akHJ"] = friendExtCount;
		v["xq0fSrw3"] = medalMaxNum;
		v["MFz8YRS6"] = campaignFlag;
		v["1JFcDr05"] = maxAchievePoint;
		v["SAb3m9wo"] = zelPerAchievePoint;
		v["KCG5f1AN"] = karmaPerAchievePoint;
		v["4ARtfF7x"] = maxAchievePointZelPerDay;
		v["K0sUIn8R"] = maxAchievePointKarmaPerDay;
		v["M1AJuFU6"] = maxAchievePointItemPerDay;
		v["p3pXbuHA"] = maxAchievementChallengeAcceptCount;
		v["kmxPgJu9"] = maxAchievementRecordChallengeAcceptCount;
		v["uALQnngx"] = tutorialGatchaID;
		v["QylsZTpE"] = parseOverDriveParam; // str
		v["OjAiNSoh"] = colosseumShopTicket;
		v["924iwrJ9"] = maxColosseumTicket;
		v["woxAcRoH"] = maxCBP;
		v["5csFoG1G"] = resetFeSkillDiaCount;
		v["DYMUxgt8"] = maxBlackListCount;
		v["deYOowYJ"] = initSummonerArmID; // str
		v["QhV0G2zu"] = maxSummonerSP; // u64
		v["W9bwut7Q"] = maxSummonerFriendPoint; // u64
		v["6id2v7eN"] = maxMultiP;
		//v["P_EGG_UNIT"] = pEggUnit;
		v["jFdW1ipx"] = dbbCrystalValues; // str
		v["7o6lcc66"] = dbbFixedSettingsValues; // str

		if (actionPointRecoverFixed > 0)
			v["hAiXsSPF"] = actionPointRecoverFixed;

		if (actionPointThreshold > 0)
			v["eRQvzLeF"] = actionPointThreshold;
	}
};
RESPONSE_NS_END
