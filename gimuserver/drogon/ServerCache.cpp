#include "App.hpp"
#include "ServerCache.hpp"

#include <gimuserver/utils/BfCrypt.hpp>
#include <gimuserver/packets/net_gumi_live.hpp>
#include <gimuserver/packets/net_featurecheck.hpp>
#include <gimuserver/packets/mst_logincampaign.hpp>
#include <gimuserver/packets/mst_versioninfo.hpp>
#include <gimuserver/packets/mst_town.hpp>
#include <gimuserver/packets/mst_dungeonkey.hpp>
#include <gimuserver/packets/mst_arena.hpp>
#include <gimuserver/packets/mst_gatcha.hpp>
#include <gimuserver/packets/mst_npc.hpp>

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
static T LoadJson(std::string_view file)
{
	T obj{};
	std::string path = getSysRoot() + std::string(file);
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

	m_feature = LoadJson<FeatureCheck>("features.json");
	m_controlCenterRsp = LoadJson<SlotGameInfoR>("brave_slots.json");

	{
		m_initrsp.loginCampagin = LoadJson<LoginCampaignMst>("login_campaign.json");
		m_initrsp.progression = LoadJson<UserLevelMst>("user_level.json");
		m_initrsp.mst = LoadJson<VersionInfo>("version_info.json");
		m_initrsp.townFacility = LoadJson<TownFacilityMst>("town_facility.json");
		m_initrsp.townFacilityLv = LoadJson<TownFacilityLvMst>("town_facility_lv.json");
		m_initrsp.townLocation = LoadJson<TownLocationMst>("town_location.json");
		m_initrsp.townLocationLv = LoadJson<TownLocationLvMst>("town_location_lv.json");
		m_initrsp.dungeonKeys = LoadJson<DungeonKeyMst>("dungeon_keys.json");
		m_initrsp.arenaRanks = LoadJson<ArenaRankMst>("arena_rank.json");
		m_initrsp.gachaEffects = LoadJson<GachaEffectMst>("gacha_effects.json");
		m_initrsp.gachas = LoadJson<GachaMst>("gacha.json");
		m_initrsp.npcs = LoadJson<NpcMst>("npc.json");
		m_initrsp.videoAdInfo = LoadJson<VideoAdInfo>("video_ads_info.json");
		m_initrsp.videoRegions = LoadJson<VideoAdRegion>("video_ads_region.json");
		m_initrsp.videoAdSlots = LoadJson<VideoAdsSlotGameInfo>("video_ads_slot_game_info.json");
		m_initrsp.bannerInfo = LoadJson <BannerInfoMst> ("banner_info.json");
		m_initrsp.excludedDungeonMissions = LoadJson<ExcludedDungeonMissionMst>("excluded_dungeon_missions.json");
		m_initrsp.extraPassiveSkills = LoadJson<ExtraPassiveSkillMst>("extra_passive_skills.json");
		m_initrsp.noticeInfo = LoadJson<NoticeInfo>("notice_info.json");
		// TODO: move this to generated per-used as there's no support for the claim
		m_initrsp.dailyTaskBonuses = LoadJson<DailyTaskBonusMst>("daily_tasks_bonus.json");
		m_initrsp.dailyTaskPrizes = LoadJson<DailyTaskPrizeMst>("daily_tasks_prizes.json");
		m_initrsp.dailyTasks = LoadJson<DailyTaskMst>("daily_tasks.json");
		// ---
	}
}
