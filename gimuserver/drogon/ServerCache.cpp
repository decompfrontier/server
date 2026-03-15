#include "App.hpp"
#include "ServerCache.hpp"
#include "ServerCacheMst.hpp"

#include <gimuserver/utils/BfCrypt.hpp>

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

/*!
* Loads a JSON from the file system.
*/
template <typename T>
static T LoadJson(std::string_view mst_root, std::string_view file)
{
	T obj{};
	std::string path = std::string(mst_root) + "/" + std::string(file);
	std::string buffer{};
	const auto& ec = glz::read_file_json(obj, path, buffer);
	if (ec)
	{
		throw std::runtime_error(std::format("Cannot read JSON file \"{}\", error:\n{}", file, glz::format_error(ec, buffer)));
	}

	return obj;
}

void ServerCache::Setup(const Json::Value& serverObj)
{
	const auto& mstRoot = serverObj["mst_root"].asString();


	{
		GameDls dls;
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
		m_initrsp.login_campagin = LoadJson<LoginCampaignMst>(mstRoot, "login_campaign.json");
		m_initrsp.login_campaign_reward = LoadJson<LoginCampaignRewardCache>(mstRoot, "login_campaign_reward.json").data;
		m_initrsp.progression = LoadJson<UserLevelMstCache>(mstRoot, "user_level.json").data;
		m_initrsp.mst = LoadJson<VersionInfoCache>(mstRoot, "version_info.json").data;
		m_initrsp.town_facility = LoadJson<TownFacilityMstCache>(mstRoot, "town_facility.json").data;
		m_initrsp.town_facility_lv = LoadJson<TownFacilityLvMstCache>(mstRoot, "town_facility_lv.json").data;
		m_initrsp.town_location = LoadJson<TownLocationMstCache>(mstRoot, "town_location.json").data;
		m_initrsp.town_location_lv = LoadJson<TownLocationLvMstCache>(mstRoot, "town_location_lv.json").data;
		m_initrsp.dungeon_keys = LoadJson<DungeonKeyMstCache>(mstRoot, "dungeon_keys.json").data;
		m_initrsp.arena_ranks = LoadJson<ArenaRankMstCache>(mstRoot, "arena_rank.json").data;
		m_initrsp.gacha_effects = LoadJson<GachaEffectMstCache>(mstRoot, "gacha_effects.json").data;
		m_initrsp.gachas = LoadJson<GachaMstCache>(mstRoot, "gacha.json").data;
		m_initrsp.npcs = LoadJson<NpcMstCache>(mstRoot, "npc.json").data;
		m_initrsp.banner_info = LoadJson <BannerInfoMstCache>(mstRoot, "banner_info.json").data;
		m_initrsp.extra_passive_skills = LoadJson<ExtraPassiveSkillMstCache>(mstRoot, "extra_passive_skills.json").data;
		m_initrsp.notice_info = LoadJson<NoticeInfo>(mstRoot, "notice_info.json");
		m_initrsp.defines = LoadJson<DefineMst>(mstRoot, "defines.json");
		m_initrsp.video_ad_slots = LoadJson<VideoAdsSlotGameInfo>(mstRoot, "video_ads_slot_game_info.json");
		m_initrsp.exp_pattern = LoadJson<UnitExpPatternMstCache>(mstRoot, "unit_exp_pattern.json").data;
		m_initrsp.receipe = LoadJson<ReceipeMstCache>(mstRoot, "receipes.json").data;
		m_initrsp.trophy = LoadJson<TrophyGroupMstCache>(mstRoot, "trophy.json").data;
		m_initrsp.trophy_grade = LoadJson<TrophyGradeMstCache>(mstRoot, "trophy_grade.json").data;
		m_initrsp.information = LoadJson<InformationMstCache>(mstRoot, "information.json").data;
		m_initrsp.help = LoadJson<HelpMstCache>(mstRoot, "help.json").data;
		m_initrsp.url = LoadJson<UrlMstCache>(mstRoot, "url.json").data;
		m_initrsp.challenge = LoadJson<ChallengeMstCache>(mstRoot, "challenge.json").data;
		m_initrsp.challenge_hr = LoadJson<ChallengeHrMstCache>(mstRoot, "challenge_hr.json").data;
		m_initrsp.challenge_mis = LoadJson<ChallengeMisMstCache>(mstRoot, "challenge_mis.json").data;
		m_initrsp.challenge_grade = LoadJson<ChallengeGradeMstCache>(mstRoot, "challenge_grade.json").data;
		m_initrsp.challenge_reward = LoadJson<ChallengeRewardMstCache>(mstRoot, "challenge_reward.json").data;
		m_initrsp.challenge_item = LoadJson<ChallengeItemMstCache>(mstRoot, "challenge_item.json").data;
		m_initrsp.interactive_banner = LoadJson<InteractiveBannerInfoMstCache>(mstRoot, "interactive_banner_info.json").data;

		// cache: UserInfo response
		m_userrsp.notice_info = m_initrsp.notice_info;
		m_userrsp.video_ad_region = LoadJson<VideoAdRegionCache>(mstRoot, "video_ad_region.json").data;
		m_userrsp.video_ad_info = LoadJson<VideoAdInfoCache>(mstRoot, "video_ad_info.json").data;
		m_userrsp.gacha_info = LoadJson<GachaInfoMstCache>(mstRoot, "gacha_info.json").data;
		m_userrsp.excluded_dungeon_missions = LoadJson<ExcludedDungeonMissionMstCache>(mstRoot, "excluded_dungeons.json").data;

		// TODO(arves): move this to generated per-used as there's no support for the claim
		m_initrsp.daily_task_bonuses = LoadJson<DailyTaskBonusMst>(mstRoot, "TEMP_daily_tasks_bonus.json");
		m_initrsp.daily_task_prizes = LoadJson<DailyTaskPrizeMstCache>(mstRoot, "TEMP_daily_tasks_prizes.json").data;
		m_initrsp.daily_tasks = LoadJson<DailyTaskMstCache>(mstRoot, "TEMP_daily_tasks.json").data;
		// ---
	}
}
