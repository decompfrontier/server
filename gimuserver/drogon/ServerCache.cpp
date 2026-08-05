#include "App.hpp"
#include "ServerCache.hpp"
#include "ServerCacheMst.hpp"

#include <gimuserver/utils/BfCrypt.hpp>
#include <gimuserver/utils/JsonFile.hpp>

/*!
* Builds a JSON
* @param[in] d Template class to write
* @return Parsed json string
*/
template <typename T>
static std::string BuildJson(const T& d)
{
	std::string buffer{};
	const auto& ec = glz::write_json(d, buffer);
	if (ec)
	{
		throw std::runtime_error("Cannot build a cache JSON, error:\n{}" + glz::format_error(ec, buffer));
	}

	return buffer;
}

void ServerCache::Setup(const Json::Value& serverObj)
{
	const auto& mstRoot = serverObj["mst_root"].asString();

	// fps_cap is the client-side render cap delivered to the offline-proxy
	// libcurl shim via the /offline_mod/fps_cap endpoint. Default 60 (matches
	// the proxy's compile-time default); 0 disables the cap entirely.
	m_serverConfig.fpsCap = serverObj.get("fps_cap", 60u).asUInt();


	{
		GameDls dls{};
		dls.game_ip = GetDrogonBindHostname();
		dls.resource_ip = dls.game_ip;
		dls.version = serverObj["game_version"].asUInt();
		dls.gumilive_ip = dls.game_ip + "/";
		dls.bg_image = serverObj["wallpaper_banner"].asString();

		const auto dlsJson = BuildJson(dls);
		const auto sree = BfCrypt::BuildSREE(dlsJson);

		// build SREE crypted JSON rather than constructing it everytime in the dls controller
		if (sree.has_value())
		{
			m_dls = BuildJson(sree.value());
		}
		else
		{
			throw std::runtime_error("Cannot encrypt SREE cache json");
		}
	}

	m_feature = LoadJson<FeatureCheck>(mstRoot, "features.json");
	m_controlCenterRsp = LoadJson<SlotGameInfoR>(mstRoot, "brave_slots.json");

	{
		// Cache: Initialize response
		m_initrsp.login_campagin = LoadJson<LoginCampaignMst>(mstRoot, "login_campaign_mst.json");
		m_initrsp.login_campaign_reward = LoadJson<LoginCampaignRewardCache>(mstRoot, "login_campaign_reward_mst.json").data;
		m_initrsp.progression = LoadJson<UserLevelMstCache>(mstRoot, "user_level_mst.json").data;
		m_initrsp.mst = LoadJson<VersionInfoCache>(mstRoot, "version_info_mst.json").data;
		m_initrsp.town_facility = LoadJson<TownFacilityMstCache>(mstRoot, "town_facility_mst.json").data;
		m_initrsp.town_facility_lv = LoadJson<TownFacilityLvMstCache>(mstRoot, "town_facility_lv_mst.json").data;
		m_initrsp.town_location = LoadJson<TownLocationMstCache>(mstRoot, "town_location_mst.json").data;
		m_initrsp.town_location_lv = LoadJson<TownLocationLvMstCache>(mstRoot, "town_location_lv_mst.json").data;
		m_initrsp.dungeon_keys = LoadJson<DungeonKeyMstCache>(mstRoot, "dungeon_key_mst.json").data;
		m_initrsp.arena_ranks = LoadJson<ArenaRankMstCache>(mstRoot, "arena_rank_mst.json").data;
		m_initrsp.gacha_effects = LoadJson<GachaEffectMstCache>(mstRoot, "gacha_effect_mst.json").data;
		m_initrsp.gachas = LoadJson<GachaMstCache>(mstRoot, "gacha_mst.json").data;
		m_initrsp.npcs = LoadJson<NpcMstCache>(mstRoot, "npc_mst.json").data;
		m_initrsp.banner_info = LoadJson <BannerInfoMstCache>(mstRoot, "banner_info_mst.json").data;
		m_initrsp.extra_passive_skills = LoadJson<ExtraPassiveSkillMstCache>(mstRoot, "extra_passive_skill_mst.json").data;
		m_initrsp.notice_info = LoadJson<NoticeInfo>(mstRoot, "notice_info.json");
		m_initrsp.defines = LoadJson<DefineMst>(mstRoot, "defines_mst.json");
		m_initrsp.video_ad_slots = LoadJson<VideoAdsSlotGameInfo>(mstRoot, "video_ad_slot_game_info_mst.json");
		m_initrsp.exp_pattern = LoadJson<UnitExpPatternMstCache>(mstRoot, "unit_exp_pattern_mst.json").data;
		m_initrsp.receipe = LoadJson<ReceipeMstCache>(mstRoot, "recipe_mst.json").data;
		m_initrsp.trophy = LoadJson<TrophyMstCache>(mstRoot, "trophy_mst.json").data;
		m_initrsp.trophy_group = LoadJson<TrophyGroupMstCache>(mstRoot, "trophy_group_mst.json").data;
		m_initrsp.trophy_grade = LoadJson<TrophyGradeMstCache>(mstRoot, "trophy_grade_mst.json").data;
		m_initrsp.information = LoadJson<InformationMstCache>(mstRoot, "information_mst.json").data;
		m_initrsp.help = LoadJson<HelpMstCache>(mstRoot, "help_mst.json").data;
		m_initrsp.help_sub = LoadJson<HelpSubMstCache>(mstRoot, "help_sub_mst.json").data;
		m_initrsp.url = LoadJson<UrlMstCache>(mstRoot, "url_mst.json").data;
		m_initrsp.challenge = LoadJson<ChallengeMstCache>(mstRoot, "challenge_mst.json").data;
		m_initrsp.challenge_hr = LoadJson<ChallengeHrMstCache>(mstRoot, "challenge_hr_mst.json").data;
		m_initrsp.challenge_mis = LoadJson<ChallengeMisMstCache>(mstRoot, "chlng_mission_mst.json").data;
		m_initrsp.challenge_grade = LoadJson<ChallengeGradeMstCache>(mstRoot, "chlng_mission_grade_mst.json").data;
		m_initrsp.challenge_reward = LoadJson<ChallengeRewardMstCache>(mstRoot, "chlng_mission_reward_mst.json").data;
		m_initrsp.challenge_item = LoadJson<ChallengeItemMstCache>(mstRoot, "chlng_mission_item_set_mst.json").data;
		m_initrsp.challenge_rank_reward = LoadJson<ChallengeRankRewardMstCache>(mstRoot, "challenge_rank_reward_mst.json").data;
		m_initrsp.challenge_mvp = LoadJson<ChallengeMvpMstCache>(mstRoot, "challenge_mvp_mst.json").data;
		m_initrsp.interactive_banner = LoadJson<InteractiveBannerInfoMstCache>(mstRoot, "interactive_banner_info_mst.json").data;
		m_initrsp.sound = LoadJson<SoundMstCache>(mstRoot, "sound_mst.json").data;
		
		// cache: UserInfo response
		m_userrsp.notice_info = m_initrsp.notice_info;
		m_userrsp.video_ad_region = LoadJson<VideoAdRegionCache>(mstRoot, "video_ad_region_mst.json").data;
		m_userrsp.video_ad_info = LoadJson<VideoAdInfoCache>(mstRoot, "video_ad_info_mst.json").data;
		m_userrsp.excluded_dungeon_missions = LoadJson<ExcludedDungeonMissionMstCache>(mstRoot, "excluded_dungeon_mission_mst.json").data;
		m_userrsp.gift = LoadJson<GiftItemMstCache>(mstRoot, "gift_item_mst.json").data;
		m_userrsp.general_event = LoadJson<GeneralEventMstCache>(mstRoot, "general_event_mst.json").data;
		m_userrsp.first_desc = LoadJson<FirstDescMstCache>(mstRoot, "first_desc_mst.json").data;
		m_userrsp.summon_ticket_v2 = LoadJson<SummonTicketV2MstCache>(mstRoot, "summon_ticket_v2_mst.json").data;
		m_userrsp.resummon_gacha = LoadJson<ResummonGachaMstCache>(mstRoot, "resummon_gacha_mst.json").data;

		m_unitMst = LoadJson<UnitMstCache>(mstRoot, "unit_mst.json").data;
		m_missionMst = LoadJson<MissionMstCache>(mstRoot, "mission_mst.json").data;
		m_itemMst = LoadJson<ItemMstCache>(mstRoot, "item_mst.json").data;

		// cache: GachaList response (gacha_info comes from GachaArchiver at
		// request time; only the category banners are cached here)
		m_gachaListRsp.gacha_categories = LoadJson<GachaCategoryCache>(mstRoot, "gacha_category_mst.json").data;

		// TODO(arves): move this to generated per-used as there's no support for the claim
		m_initrsp.daily_task_bonuses = LoadJson<DailyTaskBonusMst>(mstRoot, "daily_task_bonus_mst.json");
		m_initrsp.daily_task_prizes = LoadJson<DailyTaskPrizeMstCache>(mstRoot, "daily_task_prize_mst.json").data;
		m_initrsp.daily_tasks = LoadJson<DailyTaskMstCache>(mstRoot, "daily_task_mst.json").data;
		// ---
	}
}

