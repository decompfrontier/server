#include "MstConfig.hpp"
#include "core/System.hpp"

#include <fstream>

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

void MstConfig::LoadProgressionInfo(const std::string& path)
{
	Json::Value root;
	LoadJson(path, "progressioninfo.json", root);

	const auto& msts = root["progression_info"];

	for (const auto& v : msts)
	{
		Response::UserLevelMst::Data tbl;
		tbl.level = v["level"].asUInt();
		tbl.exp = v["exp"].asUInt();
		tbl.actionPoints = v["energy"].asUInt();
		tbl.deckCost = v["deck_cost"].asUInt();
		tbl.friendCount = v["friend_count"].asUInt();
		tbl.addFriendCount = v["add_friend_count"].asUInt();
		m_progressions.Mst.insert_or_assign(tbl.level - 1, tbl);
	}

	m_progressions.Serialize(m_initMst);
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
		id++;
	}

	tables.Serialize(res);
}

void MstConfig::LoadGacha(const std::string& path)
{
	Json::Value root;
	LoadJson(path, "gacha.json", root);

	const auto& msts = root["gacha_info"];

	Response::GachaMst tables;
	Response::GachaInfo table2;

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
		
		Response::GachaInfo::Data tbl2;
		tbl2.id = v["id"].asUInt();
		tbl2.gachaName = v["name"].asString();
		tbl2.gachaType = v["type"].asUInt();
		tbl2.priority = v["priority"].asUInt();
		tbl2.startDate = v["start_date"].asString();
		tbl2.endDate = v["end_date"].asString();
		tbl2.startHour = v["start_hour"].asString();
		tbl2.endHour = v["end_hour"].asString();
		tbl2.needFriendPoint = v["need_friend_point"].asUInt();
		tbl2.braveCoin = v["need_gems"].asUInt();
		tbl2.onceDayFlag = v["once_day_flag"].asBool() ? 1 : 0;
		tbl2.bgImg = v["bg_img"].asString();
		tbl2.btnImg = v["btn_img"].asString();
		tbl2.doorImg = v["door_img"].asString();
		tbl2.captionMsg = v["caption_msg"].asString();
		tbl2.detailMsg = v["detail_msg"].asString();
		tbl2.commentMsg = v["comment_msg"].asString();
		tbl2.gachaGroupID = v["gatcha_group_id"].asUInt();
		tbl2.description = v["description"].asString();
		tbl2.gachaType = 2; // NOTE: friend point summons = 1? rare summon= 2000?

		tables.Mst.emplace_back(tbl);
		table2.Mst.emplace_back(tbl2);
	}

	const auto& json_cats = root["gacha_categories"];
	Response::GachaCategory cats;
	for (const auto& v : json_cats)
	{
		Response::GachaCategory::Data d;
		d.id = v["id"].asUInt();
		d.img = v["img"].asString();
		d.dispOrder = v["display_order"].asUInt();
		d.startDate = v["start_date"].asUInt();
		d.endDate = v["end_date"].asUInt();
		d.gachaIDList = "";

		const auto& gachaids = v["gatcha_ids"];
		for (const auto& q : gachaids)
		{
			d.gachaIDList += std::to_string(q.asUInt()) + ",";
		}

		if (!d.gachaIDList.empty())
			d.gachaIDList.erase(d.gachaIDList.size() - 1);

		cats.Mst.emplace_back(d);
	}


	tables.Serialize(m_initMst);
	table2.Serialize(m_userInfoMst);
	table2.Serialize(m_gatchaInfo);
	cats.Serialize(m_gatchaInfo);
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

static void LoadNpcMst(const std::string& basePath, Json::Value& res)
{
	Json::Value root;
	LoadJson(basePath, "npc.json", root);

	const auto& msts = root["npcs"];

	Response::NpcMst tables;
	for (const auto& v : msts)
	{
		Response::NpcMst::Data tbl;
		tbl.id = v["id"].asUInt();
		tbl.handle_name = v["name"].asString();
		tbl.arena_rank_id = v["arena_rank_id"].asUInt();

		const auto& team = v["team_info"];
		tbl.team.friendMessage = team["friend_message"].asString();
		tbl.team.userId = team["user_id"].asString();
		tbl.team.lv = team["level"].asUInt();

		const auto& parties = v["party_info"];
		for (const auto& k : parties)
		{
			Response::NpcPartyInfo::Data d;
			d.id = k["id"].asUInt();
			d.type = k["type"].asUInt();
			d.disp_order = k["disp_order"].asUInt();
			tbl.parties.Mst.emplace_back(d);
		}

		const auto& units = v["unit_info"];
		for (const auto& k : units)
		{
			Response::NpcUnitInfo::Data d;
			d.id = k["id"].asUInt();
			d.party_id = k["party_id"].asUInt();
			d.type = k["type_id"].asUInt();
			d.lv = k["lv"].asUInt();
			d.hp = k["hp"].asUInt();
			d.atk = k["atk"].asUInt();
			d.def = k["def"].asUInt();
			d.hel = k["hel"].asUInt();
			d.skill_id = k["skill_id"].asUInt();
			d.skill_lv = k["skill_lv"].asUInt();
			d.equip_item_id = k["equip_item_id"].asUInt();
			tbl.units.Mst.emplace_back(d);
		}

		tables.Mst.emplace_back(tbl);
	}

	tables.Serialize(res);
}

static void LoadSlotInfo(const std::string& basePath, 
	Response::VideoAdsSlotgameInfo& g,
	Json::Value& userInitJson)
{
	Json::Value root;
	LoadJson(basePath, "slotinfo.json", root);

	const auto& gameinfo = root["slot_info"];

	{
		g.gameInfo.id = gameinfo["id"].asUInt();
		g.gameInfo.reelPos = gameinfo["reel_pos"].asString();
		g.gameInfo.useMedal = gameinfo["use_medal"].asString();
		g.gameInfo.slotHelpUrl = gameinfo["slot_help_url"].asString();
		g.gameInfo.slotImage = gameinfo["slot_image"].asString();

		Response::VideoAdInfo tbl;
		Response::VideoAdInfo::Data d;
		d.id = g.gameInfo.id;
		d.availableValue = gameinfo["available_value"].asUInt();
		d.videoEnabled = gameinfo["enabled"].asBool();
		d.regionId = gameinfo["region_id"].asUInt();
		d.nextAvailableTimeLeft = gameinfo["available_time_left"].asUInt();
		tbl.Mst.emplace_back(d);
		tbl.Serialize(userInitJson);
	}

	const auto& reelInfo = root["reel_info"];
	for (const auto& k : reelInfo)
	{
		Response::SlotGameReelInfo::Data d;
		d.id = k["id"].asUInt();
		d.reelData = k["data"].asString();
		g.reelInfo.Mst.emplace_back(d);
	}

	const auto& pictureInfo = root["picture_infos"];
	for (const auto& k : pictureInfo)
	{
		Response::SlotGamePictureInfo::Data d;
		d.id = k["id"].asUInt();
		d.pictureName = k["picture_name"].asString();
		g.pictureInfo.Mst.emplace_back(d);
	}

	const auto& adsInfo = root["ads_game_info"];
	{
		g.gameStandInfo.max_ads_count = adsInfo["max_ads"].asUInt();
		g.gameStandInfo.max_bonus_count = adsInfo["max_bonus_count"].asUInt();
	}

	const auto& regionInfo = root["region_info"];
	{
		Response::VideoAdRegion t;
		for (const auto& k : regionInfo)
		{
			Response::VideoAdRegion::Data d;
			d.id = k["id"].asUInt();
			d.countryCodes = k["country_codes"].asString();
			t.Mst.emplace_back(d);
		}

		t.Serialize(userInitJson);
	}

	g.CacheFields();
}

static void LoadBannerInfo(const std::string& basePath, Json::Value& res)
{
	Json::Value root;
	LoadJson(basePath, "banners.json", root);

	const auto& r = root["banners"];
	uint32_t id = 1;
	Response::BannerInfoMst m;
	for (const auto& k : r)
	{
		Response::BannerInfoMst::Data d;
		d.name = k["name"].asString();
		d.id = id;
		d.dispOrder = id - 1;
		d.url = k["url"].asString();
		d.targetOS = k["os"].asString();
		d.image = k["image"].asString();
		id++;
		m.Mst.emplace_back(d);
	}
	m.Serialize(res);
}

static void LoadExcludedDungeonsMst(const std::string& path, Json::Value& v)
{
	Json::Value root;
	LoadJson(path, "excluded_dungeons.json", root);

	const auto& p = root["list"];

	Response::ExcludedDungeonMissionMst tbl;
	for (const auto& k : p)
	{
		Response::ExcludedDungeonMissionMst::Data d;
		d.id = k["mission_id"].asUInt();
		tbl.Mst.emplace_back(d);
	}
	tbl.Serialize(v);
}

void MstConfig::LoadExtraSkillPassive(const std::string& path)
{
	Json::Value root;
	LoadJson(path, "extra_passive_skills.json", root);

	const auto& p = root["list"];

	Response::ExtraPassiveSkillMst tbl;
	for (const auto& k : p)
	{
		Response::ExtraPassiveSkillMst::Data d;
		d.id = k["skill_id"].asUInt();
		d.skillName = k["skill_name"].asString();
		d.skillNameS = k["skill_name_s"].asString();
		d.skillType = k["skill_type"].asUInt();
		d.groupID = k["group_id"].asUInt();
		d.priority = k["priority"].asUInt();
		d.rare = k["rare"].asUInt();
		d.termParam = k["term_param"].asString();
		d.target = k["target"].asUInt();
		d.processID = k["process_id"].asString();
		d.processParam = k["process_param"].asString();
		d.description = k["description"].asString();
		tbl.Mst.emplace_back(d);
	}
	tbl.Serialize(m_initMst);
}

void MstConfig::LoadUrl(const std::string& path)
{
	Json::Value root;
	LoadJson(path, "urls.json", root);

	Response::UrlMst m;
	m.officialSite = root["official_site"].asString();
	m.noticeUrl = root["official_site"].asString();
	m.contactUrl = root["contact_url"].asString();
	m.friendRefeerUrl = root["friend_refeer_url"].asString();
	m.appliDlUrl = root["appli_dl_url"].asString();
	m.appliDlAndroidUrl = root["appli_dl_android_url"].asString();
	m.famiAppSiteUrl = root["fami_appsite_url"].asString();
	m.twitterSiteUrl = root["twitter_url"].asString();
	m.facebookSiteUrl = root["facebook_url"].asString();
	m.transferSiteUrl = root["transfer_site_url"].asString();
	m.appBankSiteUrl = root["appbank_site_url"].asString();
	m.loblSiteUrl = root["lobl_site_url"].asString();
	m.loblSchemaUrl = root["lobl_schema_url"].asString();
	m.appliSommelierUrl = root["appli_sommelier_url"].asString();
	m.creditUrl = root["credit_url"].asString();
	m.gameGiftUrl = root["gamegift_url"].asString();
	m.agreementUrl = root["agreement_url"].asString();
	m.agreementOfficialUrl = root["agreement_official_url"].asString();
	m.legalfundSettlementUrl = root["legalfund_settlement_url"].asString();
	m.specificTradeUrl = root["specific_trade_url"].asString();
	m.diaPossessionUrl = root["dia_possession_url"].asString();
	m.lobiRecHelpUrl = root["lobi_rechelp_url"].asString();
	m.lobiAgreementUrl = root["lobi_agreement_url"].asString();
	m.gachaContentsUrl = root["gachacontents_url"].asString();
	m.multiArchiveUrl = root["multiarchive_url"].asString();

	m.Serialize(m_initMst);
}


void MstConfig::LoadAllTables(std::string_view basePath)
{
	LoadLoginCampaignMst(basePath, m_initMst);
	LoadProgressionInfo(basePath);
	LoadMstInfo(basePath, m_initMst);
	LoadTownFacility(basePath, m_initMst);
	LoadTownLocation(basePath, m_initMst);
	LoadDungeonKeys(basePath, m_initMst);
	LoadArenaRank(basePath, m_initMst);
	LoadGachaEffects(basePath, m_initMst);
	LoadGacha(basePath);
	LoadDefine(basePath, m_initMst);
	LoadNpcMst(basePath, m_initMst);
	LoadSlotInfo(basePath, m_videoAdsSlot, m_userInfoMst);
	LoadBannerInfo(basePath, m_initMst);
	LoadExcludedDungeonsMst(basePath, m_userInfoMst);
	LoadExtraSkillPassive(basePath);
	m_dailyTask.LoadTableFromJson(basePath);
	m_startInfo.LoadTableFromJson(basePath);

	// precompute extra data to save time
	{
		Response::NoticeInfo n;
		n.url = System::Instance().ServerConfig().NoticeUrl;
		n.Serialize(m_initMst);
		n.Serialize(m_userInfoMst);
	}
}
