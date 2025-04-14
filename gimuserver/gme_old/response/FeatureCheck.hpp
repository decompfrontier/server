#pragma once

#include "../GmeRequest.hpp"

RESPONSE_NS_BEGIN
struct FeatureCheck : public IResponse
{
	explicit FeatureCheck() : randall(true), frontierhunter(true),
		banner_mst(true), fish(true), facebook_stories(false),
		autobattle(true), multiaccept(true),
		felloplay_community(true), raid(true),
		shop_friend(true), slot(true), sort(true),
		ischeat_enable(false), supersonic_google(false),
		google_felloplay(false), corrupted_mst_check(false),
		tutorial_skip(true), bf_campaign_grand_quest(true),
		bf_achievement(true), bf_achievement_ext(true),
		arx_popup_enable(true), arx_popup_ios_enable(true),
		felloplay_community_ios(true),
		feature_gate(true), challenge_arena(false),
		battle_speed(true), battle_speed_ca(true),
		autobattle_record(true), sandbag_enable(true),
		colosseum_enable(true), video_ads(true),
		challenge_arena_banner_lock(true), guild_visible(true),
		guild(true), name_change_func(true), video_ads_slot(true),
		new_164_trial(true), amazon_conins_reward_control(false),
		mystery_chest(true), va_sp_skill(true), frontiergate_plus(true),
		freepaid_gems(true), battle_speed_arena_pvp(true), gacha_category(true),
		unit_type_bonus_skill(true), battle_item_limit(500),
		dungeon_key_cnt(1), cooldown_timer(5)
	{
		daily_dungeon_list = "100000,100050,100100,100200,100300,100310,100320,100400,100500";
	}


	bool randall, frontierhunter,
		banner_mst, fish, facebook_stories,
		autobattle, multiaccept,
		felloplay_community, raid,
		shop_friend, slot, sort,
		ischeat_enable, supersonic_google,
		google_felloplay, corrupted_mst_check,
		tutorial_skip, bf_campaign_grand_quest,
		bf_achievement, bf_achievement_ext,
		arx_popup_enable, arx_popup_ios_enable,
		felloplay_community_ios,
		feature_gate, challenge_arena,
		battle_speed, battle_speed_ca,
		autobattle_record, sandbag_enable,
		colosseum_enable, video_ads,
		challenge_arena_banner_lock, guild_visible,
		guild, name_change_func, video_ads_slot,
		new_164_trial, amazon_conins_reward_control,
		mystery_chest, va_sp_skill, frontiergate_plus,
		freepaid_gems, battle_speed_arena_pvp, gacha_category,
		unit_type_bonus_skill;

	uint32_t battle_item_limit,
		dungeon_key_cnt,
		cooldown_timer;

	std::string daily_dungeon_list;

	bool isArray() const override { return false; }
	const char* getGroupName() const override { return nullptr; }

protected:
	void SerializeFields(Json::Value& v, size_t) const override
	{
		v["randall"] = randall ? 1 : 0;
		v["frontierhunter"] = frontierhunter ? 1 : 0;
		
		// new
		v["dungeon_key"] = 1;
		v["exp_window"] = 1;
		v["trial"] = 1;
		// end of new

		v["banner_mst"] = banner_mst ? 1 : 0;

		// new
		v["reload_file_mst"] = 0;
		v["recommend_app"] = 0;
		// end of new

		v["raid"] = raid ? 1 : 0;

		// new

		v["raid_beta"] = 0;
		v["dict"] = 0;
		v["character_voice"] = 1;
		v["raid_battle_restart"] = 1;
		// end of new

		v["autobattle"] = autobattle ? 1 : 0;

		// new
		v["multisummon"] = 1;
		// end of new

		v["multiaccept"] = multiaccept ? 1 : 0;
		v["facebook_stories"] = facebook_stories ? 1 : 0;
		v["name_change_func"] = name_change_func ? 1 : 0;

		// new
		v["randall_facility"] = 1;
		v["dailytask_notify"] = 1;
		v["dailylogin_gem"] = 1;
		// end of new

		v["shop_friend"] = shop_friend ? 1 : 0;
		v["slot"] = slot ? 1 : 0;
		v["sort"] = sort ? 1 : 0;
		v["dungeon_key_cnt"] = dungeon_key_cnt;

		// new
		v["dlc_popup_android"] = 1;
		v["select_dlc_android"] = 1;
		//v["fps_low"] = 30.0;
		// end of new

		v["battle_item_limit"] = battle_item_limit;

		// new
		//v["bb_timer"] = 4;
		v["social_special"] = 1;
		// end of new

		v["ischeat_enable"] = ischeat_enable ? 1 : 0;

		// new
		v["arx_punish_enable"] = 1;
		// end of new

		v["arx_popup_enable"] = arx_popup_enable ? 1 : 0;
		v["arx_popup_ios_enable"] = arx_popup_ios_enable ? 1 : 0;

		// new
		v["randall_library_memories"] = 1;
		v["full_unit_ills_esclude"] = 0;
		// end of new
		
		v["corrupted_mst_check"] = corrupted_mst_check ? 1 : 0;
		v["tutorial_skip"] = tutorial_skip ? 1 : 0;
		v["bf_campaign_grand_quest"] = bf_campaign_grand_quest ? 1 : 0;
		v["bf_achievement"] = bf_achievement ? 1 : 0;
		v["bf_achievement_ext"] = bf_achievement_ext ? 1 : 0;

		// new
		v["force_use_summon_tickets"] = 0;
		// end of new

		v["feature_gate"] = feature_gate ? 1 : 0;
		v["challenge_arena"] = challenge_arena ? 1 : 0;
		v["challenge_arena_banner_lock"] = challenge_arena_banner_lock ? 1 : 0;
		v["video_ads"] = video_ads ? 1 : 0;
		v["video_ads_slot"] = video_ads_slot ? 1 : 0;
		v["battle_speed"] = battle_speed ? 1 : 0;
		v["battle_speed_ca"] = battle_speed_ca ? 1 : 0;
		v["battle_speed_arena_pvp"] = battle_speed_arena_pvp ? 1 : 0;
		v["autobattle_record"] = autobattle_record ? 1 : 0;
		v["colosseum_enable"] = colosseum_enable ? 1 : 0;
		v["sandbag_enable"] = sandbag_enable ? 1 : 0;

		// new
		v["exclude_ca_fusion_unit"] = 0;
		v["sg_resummon_gacha_enable"] = 1;
		v["sg_target_bundle_flag"] = 1;
		// end of new

		v["guild_visible"] = guild_visible ? 1 : 0;
		v["guild"] = guild ? 1 : 0;

		// new
		v["old_video_ads_slot"] = 0;
		// end of new

		v["daily_dungeon_list"] = daily_dungeon_list;
		v["new_164_trial"] = new_164_trial ? 1 : 0;
		v["mystery_chest"] = mystery_chest ? 1 : 0;
		v["freepaid_gems"] = freepaid_gems ? 1 : 0;
		v["cooldown_timer"] = cooldown_timer;
		v["va_sp_skill"] = va_sp_skill ? 1 : 0;
		v["frontiergate_plus"] = frontiergate_plus ? 1 : 0;
		v["gacha_category"] = gacha_category ? 1 : 0;
		v["unit_type_bonus_skill"] = unit_type_bonus_skill ? 1 : 0;

		// unused by 2.9.16

		v["fish"] = fish ? 1 : 0;
		v["felloplay_community"] = felloplay_community ? 1 : 0;
		v["supersonic_google"] = supersonic_google ? 1 : 0;
		v["google_felloplay"] = google_felloplay ? 1 : 0;
		v["felloplay_community_ios"] = felloplay_community_ios ? 1 : 0;
		v["amazon_conins_reward_control"] = amazon_conins_reward_control ? 1 : 0;
	}
};
RESPONSE_NS_END
