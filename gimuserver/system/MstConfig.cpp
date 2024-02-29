#include "MstConfig.hpp"
#include <fstream>

#include "gme/response/LoginCampaignMst.hpp"
#include "gme/response/LoginCampaignReward.hpp"
#include "gme/response/VersionInfo.hpp"
#include "gme/response/UserLevelMst.hpp"
#include "gme/response/TownFacilityLvMst.hpp"
#include "gme/response/TownFacilityMst.hpp"
#include "gme/response/TownLocationLvMst.hpp"
#include "gme/response/TownLocationMst.hpp"
#include "gme/response/GachaMst.hpp"
#include "gme/response/UnitExpPatternMst.hpp"
#include "gme/response/GachaEffectMst.hpp"
#include "gme/response/DefineMst.hpp"
#include "gme/response/DungeonKeyMst.hpp"
#include "gme/response/ArenaRankMst.hpp"
#include "gme/response/NpcMst.hpp"

static void LoadJson(const std::string& path, const std::string& name, Json::Value& root)
{
	std::ifstream ifs;
	ifs.open(path + "/" + name);

	if (!ifs.is_open())
		throw std::runtime_error(name + ": not found");

	Json::CharReaderBuilder rb;
	JSONCPP_STRING jss;
	if (!Json::parseFromStream(rb, ifs, &root, &jss))
		throw std::runtime_error(name + ": parse error");
}

static void LoadLoginCampaignMst(const std::string& path, Json::Value& res)
{
	Json::Value root;
	LoadJson(path, "logincampaign.json", root);

	Response::LoginCampaignMst camp;

	camp.image = root["login_banner"].asCString();
	const std::string icon = root["login_icon"].asCString();
	camp.startDate = root["start_time"].asUInt();
	camp.totalDays = root["duration"].asUInt();
	const auto icon_days = root["banner_icon_days"].asUInt();

	std::string icon_prefix;
	std::string icon_suffix = "";

	auto p = icon.find('@');
	if (p == std::string::npos)
	{
		icon_prefix = icon;
	}
	else
	{
		icon_prefix = icon.substr(0, p);
		icon_suffix = icon.substr(p + 1);
	}

	Response::LoginCampaignReward rew;
	for (uint32_t i = 1; i <= icon_days; i++)
	{
		Response::LoginCampaignReward::Data d;
		d.id = camp.id;
		d.reward_day = i;
		d.reward_img = icon_prefix + std::to_string(i) + icon_suffix;
		rew.Mst.emplace_back(d);
	}

	camp.Serialize(res);
	rew.Serialize(res);
}

static void LoadMstInfo(const std::string& path, Json::Value& res)
{
	Json::Value root;
	LoadJson(path, "mstinfo.json", root);

	const auto& msts = root["MSTs"];

	Response::VersionInfo tables;
	for (const auto& v : msts)
	{
		Response::VersionInfo::Data tbl;
		tbl.Id = v["Name"].asCString();
		tbl.Version = v["Version"].asInt();
		tbl.Unknown = v["Unknown"].asInt();
		tbl.SubVersion = v["SubVersion"].asInt();
		tables.Mst.emplace_back(tbl);
	}

	tables.Serialize(res);
}

static void LoadProgressionInfo(const std::string& path, Json::Value& res)
{
	Json::Value root;
	LoadJson(path, "progressioninfo.json", root);

	const auto& msts = root["progression_info"];

	Response::UserLevelMst tables;
	for (const auto& v : msts)
	{
		Response::UserLevelMst::Data tbl;
		tbl.level = v["level"].asUInt();
		tbl.exp = v["exp"].asUInt();
		tbl.actionPoints = v["energy"].asUInt();
		tbl.deckCost = v["deck_cost"].asUInt();
		tbl.friendCount = v["friend_count"].asUInt();
		tbl.addFriendCount = v["add_friend_count"].asUInt();
		tables.Mst.emplace_back(tbl);
	}

	tables.Serialize(res);
}

static void LoadTownFacility(const std::string& path, Json::Value& res)
{
	Json::Value root;
	LoadJson(path, "town_facility.json", root);

	{
		const auto& msts = root["info"];

		Response::TownFacilityMst tables;
		for (const auto& v : msts)
		{
			Response::TownFacilityMst::Data tbl;
			tbl.id = v["id"].asUInt();
			tbl.pos_x = v["pos_x"].asUInt();
			tbl.pos_y = v["pos_y"].asUInt();
			tbl.width = v["width"].asUInt();
			tbl.height = v["height"].asUInt();
			tbl.need_mission_id = v["need_mission_id"].asUInt();
			tbl.name = v["name"].asString();
			tables.Mst.emplace_back(tbl);
		}

		tables.Serialize(res);
	}

	{
		const auto& msts = root["levels"];

		Response::TownFacilityLvMst tables;
		for (const auto& v : msts)
		{
			Response::TownFacilityLvMst::Data tbl;
			tbl.id = v["id"].asUInt();
			tbl.lv = v["lv"].asUInt();
			tbl.karma = v["karma"].asUInt();
			tbl.release_receipe = v["release_receipe"].asString();
			tables.Mst.emplace_back(tbl);
		}

		tables.Serialize(res);
	}
}

static void LoadTownLocation(const std::string& path, Json::Value& res)
{
	Json::Value root;
	LoadJson(path, "town_location.json", root);

	{
		const auto& msts = root["info"];

		Response::TownLocationMst tables;
		for (const auto& v : msts)
		{
			Response::TownLocationMst::Data tbl;
			tbl.id = v["id"].asUInt();
			tbl.pos_x = v["pos_x"].asUInt();
			tbl.pos_y = v["pos_y"].asUInt();
			tbl.width = v["width"].asUInt();
			tbl.height = v["height"].asUInt();
			tbl.need_mission_id = v["need_mission_id"].asUInt();
			tbl.name = v["name"].asString();
			tables.Mst.emplace_back(tbl);
		}

		tables.Serialize(res);
	}

	{
		const auto& msts = root["levels"];

		Response::TownLocationLvMst tables;
		for (const auto& v : msts)
		{
			Response::TownLocationLvMst::Data tbl;
			tbl.id = v["id"].asUInt();
			tbl.lv = v["lv"].asUInt();
			tbl.karma = v["karma"].asUInt();
			tbl.release_receipe = v["release_receipe"].asString();
			tables.Mst.emplace_back(tbl);
		}

		tables.Serialize(res);
	}
}

static void LoadDungeonKeys(const std::string& path, Json::Value& res)
{
	Json::Value root;
	LoadJson(path, "dungeon_keys.json", root);

	const auto& msts = root["dungeon_keys"];

	Response::DungeonKeyMst tables;
	for (const auto& v : msts)
	{
		Response::DungeonKeyMst::Data tbl;
		tbl.id = v["id"].asInt();
		tbl.name = v["name"].asString();
		tbl.dungeonId = v["dungeon_id"].asInt();
		tbl.thumImg = v["thum_img"].asString();
		tbl.openImg = v["open_img"].asString();
		tbl.closeImg = v["close_img"].asString();
		tbl.limitSec = v["limit_sec"].asInt();
		tbl.possessionLimit = v["possession_limit"].asInt();
		tbl.distributeFlag = v["distribute_flag"].asInt();
		tbl.usagePattern = v["usage_pattern"].asString();
		tbl.state = v["state"].asInt();
		tables.Mst.emplace_back(tbl);
	}

	tables.Serialize(res);
}

static void LoadArenaRank(const std::string& path, Json::Value& res)
{
	Json::Value root;
	LoadJson(path, "arena_rank.json", root);

	const auto& msts = root["arena_ranks"];

	Response::ArenaRankMst tables;
	for (const auto& v : msts)
	{
		Response::ArenaRankMst::Data tbl;
		tbl.id = v["id"].asInt();
		tbl.name = v["name"].asString();
		tbl.rank_point_start = v["rank_point_start"].asInt();
		tbl.rank_point_start = v["rank_point_end"].asInt();
		tbl.reward_type = v["reward_type"].asInt();
		tbl.reward_param = v["reward_param"].asString();
		tbl.scenario_info = v["scenario_info"].asString();
		tables.Mst.emplace_back(tbl);
	}

	tables.Serialize(res);
}

static void LoadGachaEffects(const std::string& path, Json::Value& res)
{
	Json::Value root;
	LoadJson(path, "gacha_effects.json", root);

	const auto& msts = root["gacha_effects"];

	Response::GachaEffectMst tables;
	int id = 1;
	for (const auto& v : msts)
	{
		Response::GachaEffectMst::Data tbl;
		tbl.id = id;
		tbl.gatcha_id = v["gacha_id"].asInt();
		tbl.rare = v["rare"].asInt();
		tbl.rate = v["rate"].asFloat();
		tbl.effect_before = v["effect_before"].asString();
		tbl.effect_after = v["effect_after"].asString();
		tbl.effect = v["effect"].asString();
		tables.Mst.emplace_back(tbl);
	}

	tables.Serialize(res);
}

static void LoadGacha(const std::string& path, Json::Value& res)
{
	Json::Value root;
	LoadJson(path, "gacha.json", root);

	const auto& msts = root["gacha_info"];

	Response::GachaMst tables;
	for (const auto& v : msts)
	{
		Response::GachaMst::Data tbl;
		tbl.id = v["id"].asUInt();
		tbl.name = v["name"].asString();
		tbl.type = v["type"].asUInt();
		tbl.priority = v["priority"].asUInt();
		tbl.start_date = v["start_date"].asString();
		tbl.end_date = v["end_date"].asString();
		tbl.start_hour = v["start_hour"].asString();
		tbl.end_hour = v["end_hour"].asString();
		tbl.need_friend_point = v["need_friend_point"].asUInt();
		tbl.need_gems = v["need_gems"].asUInt();
		tbl.once_day_flag = v["once_day_flag"].asBool() ? 1 : 0;
		tbl.bg_img = v["bg_img"].asString();
		tbl.btn_img = v["btn_img"].asString();
		tbl.door_img = v["door_img"].asString();
		tbl.caption_msg = v["caption_msg"].asString();
		tbl.detail_msg = v["detail_msg"].asString();
		tbl.comment_msg = v["comment_msg"].asString();
		tbl.gatcha_group_id = v["gatcha_group_id"].asUInt();
		tbl.description = v["description"].asString();
		
		tables.Mst.emplace_back(tbl);
	}

	tables.Serialize(res);
}

static void LoadDefine(const std::string& path, Json::Value& res)
{
	Json::Value v;
	LoadJson(path, "define.json", v);

	Response::DefineMst tbl;
	tbl.maxZel = v["max_zel"].asUInt64();
	tbl.maxKarma = v["max_karma"].asUInt64();
	tbl.maxFriendPoint = v["max_friend_point"].asUInt64();
	tbl.maxSummonerFriendPoint = v["max_summoner_friend_point"].asUInt64();
	tbl.maxSummonerSP = v["max_summoner_sp"].asUInt64();
	tbl.friendPointReinFriendRate = v["friendpoint_rein_friend_rate"].asFloat();
	tbl.friendPointReinFriendMyRate = v["friendpoint_rein_friend_my_rate"].asFloat();
	tbl.friendPointReinOtherRate = v["friendpoint_rein_other_rate"].asFloat();
	tbl.friendPointReinOtherMyRate = v["friendpoint_rein_other_my_rate"].asFloat();
	tbl.unitMixSuperExpRate = v["unit_mix_super_exp_rate"].asFloat();
	tbl.unitMixGreatExpRate = v["unit_mix_great_exp_rate"].asFloat();
	tbl.maxTeamLv = v["max_team_lv"].asUInt();
	tbl.maxArenaRank = v["max_arena_rank"].asUInt();
	tbl.maxUnitCount = v["max_unit_count"].asUInt();
	tbl.frinedPointReinFriend = v["friendpoint_rein_friend"].asUInt();
	tbl.friendPointReinFriendMy = v["friendpoint_rein_friend_my"].asUInt();
	tbl.maxWarehouseCount = v["max_warehouse_count"].asUInt();
	tbl.defaultUnitCount = v["default_unit_count"].asUInt();
	tbl.friendPointReinOther = v["friendpoint_rein_other"].asUInt();
	tbl.defaultWarehouseCount = v["default_warehouse_count"].asUInt();
	tbl.friendPointReinOtherMy = v["friendpoint_rein_other_my"].asUInt();
	tbl.permitInvitation = v["permit_invitation"].asUInt();
	tbl.extPlusMaxCount = v["ext_plus_max_count"].asUInt();
	tbl.actionPointHealCount = v["action_point_heal_count"].asUInt();
	tbl.fightPointHealCount = v["fight_point_heal_count"].asUInt();
	tbl.unitBoxExtCount = v["unit_box_ext_count"].asUInt();
	tbl.continueDiaCount = v["continue_dia_count"].asUInt();
	tbl.itemBoxExtCount = v["item_box_ext_count"].asUInt();
	tbl.tutorialDungeonID = v["tutorial_dungeon_id"].asUInt();
	tbl.recoverTimeAction = v["recover_time_action"].asUInt();
	tbl.verifyFlag = v["verify_flag"].asUInt();
	tbl.arenaBattleTimeLimit = v["arena_battle_time_limit"].asUInt();
	tbl.recoverTimeFight = v["recover_time_fight"].asUInt();
	tbl.arenaNeedMissionID = v["arena_need_mission_id"].asUInt();
	tbl.maxPartyDeckCount = v["max_party_deck_count"].asUInt();
	tbl.recoverTimeFrohun = v["recover_time_frohun"].asUInt();
	tbl.recoverTimeRaid = v["recover_time_raid"].asUInt();
	tbl.raidMaxP = v["raid_max_p"].asUInt();
	tbl.raidBagCount = v["raid_bag_count"].asUInt();
	tbl.friendExtCount = v["friend_ext_count"].asUInt();
	tbl.medalMaxNum = v["medal_max_num"].asUInt();
	tbl.campaignFlag = v["campaign_flag"].asString();
	tbl.maxAchievePoint = v["max_achieve_point"].asUInt();
	tbl.zelPerAchievePoint = v["zel_per_archieve_point"].asUInt();
	tbl.karmaPerAchievePoint = v["karma_per_achieve_point"].asUInt();
	tbl.maxAchievePointZelPerDay = v["max_achieve_point_zel_per_day"].asUInt();
	tbl.maxAchievePointKarmaPerDay = v["max_achieve_point_karma_per_day"].asUInt();
	tbl.maxAchievePointItemPerDay = v["max_achieve_point_item_per_day"].asUInt();
	tbl.maxAchievementChallengeAcceptCount = v["max_achievement_challenge_accept_count"].asUInt();
	tbl.maxAchievementRecordChallengeAcceptCount = v["max_achievement_record_challenge_accept_count"].asUInt();
	tbl.tutorialGatchaID = v["tutorial_gatcha_id"].asUInt();
	tbl.colosseumShopTicket = v["colosseum_shop_ticket"].asUInt();
	tbl.maxColosseumTicket = v["max_colosseum_ticket"].asUInt();
	tbl.maxCBP = v["max_cbp"].asUInt();
	tbl.resetFeSkillDiaCount = v["reset_fe_skill_dia_count"].asUInt();
	tbl.maxBlackListCount = v["max_blacklist_count"].asUInt();
	tbl.maxMultiP = v["max_multi_p"].asUInt();
	tbl.dbbCrystalValues = v["dbb_crystal_values"].asString();
	tbl.dbbFixedSettingsValues = v["dbb_fixed_settings_values"].asString();
	tbl.initialUnit = v["initial_unit"].asString();
	tbl.arenaTutorialNpcInfo = v["arena_tutorial_npc_info"].asString();
	tbl.parseOverDriveParam = v["overdrive_param"].asString();
	tbl.initSummonerArmID = v["init_summoner_arm_id"].asString();

	tbl.Serialize(res);
}

void MstConfig::LoadAllTables(const std::string& basePath)
{
	LoadLoginCampaignMst(basePath, m_convMst);
	LoadProgressionInfo(basePath, m_convMst);
	LoadMstInfo(basePath, m_convMst);
	LoadTownFacility(basePath, m_convMst);
	LoadTownLocation(basePath, m_convMst);
	LoadDungeonKeys(basePath, m_convMst);
	LoadArenaRank(basePath, m_convMst);
	LoadGachaEffects(basePath, m_convMst);
	LoadGacha(basePath, m_convMst);
	LoadDefine(basePath, m_convMst);
	m_dailyTask.LoadTableFromJson(basePath);
	m_startInfo.LoadTableFromJson(basePath);
}

void MstConfig::CopyTo(Json::Value& v) const
{
	for (auto srcIt = m_convMst.begin(); srcIt != m_convMst.end(); ++srcIt)
		v[srcIt.name()] = *srcIt;
}

