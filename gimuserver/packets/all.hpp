#pragma once

#include <pkgen_helpers.hpp>


// This file is auto-generated from a KDL specification by `packet-generator`.
// Please do not modify this, but instead change the original definitions.

enum class GmeErrorFlags;

enum class GmeErrorCommand;

enum class RewardType;

/// Types of operative systems
namespace BannerOperativeSystem {
    using Type = std::string;

    /// Android System
    constexpr const auto Android = "2";
};

/// Result of the login status
namespace StatusEnum {
    using Type = std::string;

    /// Operation completed successfully
    constexpr const auto Success = "successful";

    /// Operation failed
    constexpr const auto Error = "error";
};

/// Platforms that runs the game
namespace PlatformEnum {
    using Type = std::string;

    /// Android platform
    constexpr const auto Android = "Android";

    /// Windows platform
    constexpr const auto Windows = "Windows";
};

/// Supported operative systems
namespace GameTargetOs {
    using Type = std::string;

    /// Android platform
    constexpr const auto Android = "2";

    /// Windows 8.1 (Windows RT) platform
    constexpr const auto WindowsRt = "7";
};

/// Supported game languages
namespace GameLanguages {
    using Type = std::string;

    /// English
    constexpr const auto English = "en";
};

struct GeneralEventMst;

struct FirstDescMst;

struct GiftItemMst;

struct ReceipeMst;

struct UnitExpPatternMst;

struct UnitMst;

struct UrlMst;

struct BadgeInfo;

struct FeatureCheck;

struct FeatureGatingInfo;

struct FriendPointInfo;

struct GmeHeader;

struct GmeBody;

struct GmeError;

struct GmeAction;

struct VersionInfo;

struct LoginCampaignMst;

struct LoginCampaignReward;

struct UserLevelMst;

struct TownFacilityLvMst;

struct TownLocationLvMst;

struct TownFacilityMst;

struct TownLocationMst;

struct DungeonKeyMst;

struct ArenaRankMst;

struct GachaEffectMst;

struct GachaCategory;

struct GachaMst;

struct ResummonGachaMst;

struct SummonTicketV2Mst;

struct DefineMst;

struct NpcPartyInfo;

struct NpcTeamInfo;

struct NpcUnitInfo;

struct NpcMst;

struct SlotGameInfo;

struct SlotGamePictureInfo;

struct SlotGameInfoR;

struct SlotGameReelInfo;

struct VideoAdInfo;

struct VideoAdRegion;

struct VideoAdsSlotGameStandInfo;

struct VideoAdsSlotGameInfo;

struct BannerInfoMst;

struct ExcludedDungeonMissionMst;

struct ExtraPassiveSkillMst;

struct NoticeInfo;

struct DailyTaskBonusMst;

struct DailyTaskMst;

struct DailyTaskPrizeMst;

struct SignalKey;

struct ChallengeArenaUserInfo;

struct GuildInfo;

struct DailyLoginRewardsUserInfo;

struct SummonerJournalUserInfo;

struct GuestLogin;

struct GuestLoginReq;

struct GameDls;

struct Sree;

struct GumiLiveInfo;

struct UserInfoResp;

struct UserPartyDeckInfo;

struct UserTeamInfo;

struct UserUnitInfo;

struct UserInfoReq;

struct MstUrlList;

struct InitializeReq;

struct InitializeResp;

struct ItemFavorite;

struct UserLoginCampaignInfo;

struct MissionBreakInfo;

struct NpcMessageOverwriteInfo;

struct PermitPlace;

struct PermitPlaceML;

struct PermitPlaceSP;

struct RaidUserInfo;

struct ReinformenetInfo;

struct SummonTicketV2UserInfo;


/// TODO
struct SummonTicketV2UserInfo {

};


/// TODO
struct ReinformenetInfo {

};


/// TODO
struct RaidUserInfo {

};


/// TODO
struct PermitPlaceSp {

};


/// TODO
struct PermitPlaceMl {

};


/// TODO
struct PermitPlace {

};


/// TODO
struct NpcMessageOverwriteInfo {

};


/// TODO
struct MissionBreakInfo {

};


/// TODO
struct UserLoginCampaignInfo {

    /// TODO
    int32_t id;

    /// TODO
    int32_t current_day;

    /// TODO
    int32_t total_days;

    /// TODO
    bool first_for_the_day;
};


/// TODO
struct ItemFavorite {

};


/// TODO
struct MstUrlList {

    /// Normal (not crypted) name of the MST
    std::string id;

    /// Saved MST version in the game
    int32_t version;
};


/// TODO
struct UserUnitInfo {

    /// TODO
    std::string user_id;

    /// TODO
    int32_t user_unit_id;

    /// TODO
    int32_t unit_id;

    /// TODO
    int32_t unit_type_id;

    /// TODO
    int32_t unit_lv;

    /// TODO
    int32_t exp;

    /// TODO
    int32_t total_exp;

    /// TODO
    int32_t base_hp;

    /// TODO
    int32_t add_hp;

    /// TODO
    int32_t ext_hp;

    /// TODO
    int32_t limit_over_hp;

    /// TODO
    int32_t base_atk;

    /// TODO
    int32_t add_atk;

    /// TODO
    int32_t ext_atk;

    /// TODO
    int32_t limit_over_atk;

    /// TODO
    int32_t base_def;

    /// TODO
    int32_t add_def;

    /// TODO
    int32_t ext_def;

    /// TODO
    int32_t limit_over_def;

    /// TODO
    int32_t base_heal;

    /// TODO
    int32_t add_heal;

    /// TODO
    int32_t ext_heal;

    /// TODO
    int32_t limit_over_heal;

    /// TODO
    std::string element;

    /// TODO
    int32_t leader_skill_id;

    /// TODO
    int32_t skill_id;

    /// TODO
    int32_t skill_lv;

    /// TODO
    int32_t extra_skill_id;

    /// TODO
    int32_t extra_skill_lv;

    /// TODO
    int32_t receive_date;

    /// TODO
    int32_t ext_count;

    /// TODO
    int32_t equipitem_frame_id;

    /// TODO
    int32_t equipitem_id;

    /// TODO
    int32_t equipitem_frame_id2;

    /// TODO
    int32_t equipitem_id2;

    /// TODO
    int32_t new_flag;

    /// TODO
    int32_t extra_passive_skill_id;

    /// TODO
    int32_t extra_passive_skill_id2;

    /// TODO
    int32_t add_extra_passive_skill_id;

    /// TODO
    int32_t unit_img_type;

    /// TODO
    int32_t fe_bp;

    /// TODO
    int32_t fe_used_bp;

    /// TODO
    int32_t fe_max_usable_bp;

    /// TODO
    std::string fe_skill_info;

    /// TODO
    int32_t omni_level;
};


/// TODO
struct UserTeamInfo {

    /// TODO
    std::string user_id;

    /// TODO
    int32_t level;

    /// TODO
    int64_t exp;

    /// TODO
    int32_t max_action_point;

    /// TODO
    int32_t action_point;

    /// TODO
    int32_t max_fight_point;

    /// TODO
    int32_t fight_point;

    /// TODO
    int32_t max_unit_count;

    /// TODO
    int32_t add_unit_count;

    /// TODO
    int32_t deck_cost;

    /// TODO
    int32_t max_equip_slot_count;

    /// TODO
    int32_t max_friend_count;

    /// TODO
    int32_t add_friend_count;

    /// TODO
    int32_t friend_point;

    /// TODO
    int64_t zel;

    /// TODO
    int64_t karma;

    /// TODO
    int32_t brave_coin;

    /// TODO
    std::string friend_message;

    /// TODO
    int32_t warehouse_count;

    /// TODO
    int32_t add_warehouse_count;

    /// TODO
    std::string want_gift;

    /// TODO
    int32_t present_count;

    /// TODO
    int32_t friend_agree_count;

    /// TODO
    int32_t gift_receive_count;

    /// TODO
    int32_t action_rest_timer;

    /// TODO
    int32_t fight_rest_timer;

    /// TODO
    int32_t free_gems;

    /// TODO
    int32_t active_deck;

    /// TODO
    int32_t summon_ticket;

    /// TODO
    int32_t slot_game_flag;

    /// TODO
    int32_t rainbow_coin;

    /// TODO
    int32_t brave_points_total;

    /// TODO
    int32_t colosseum_ticket;

    /// TODO
    int32_t arena_deck_num;

    /// TODO
    std::string reinforcement_deck;

    /// TODO
    int32_t paid_gems;

    /// TODO
    int32_t mysterybox_count;

    /// TODO
    int32_t completed_task_count;

    /// TODO
    int32_t inbox_message_count;

    /// TODO
    int32_t current_brave_points;
};


/// TODO
struct UserPartyDeckInfo {

    /// TODO
    int32_t deck_type;

    /// TODO
    int32_t deck_num;

    /// TODO
    int32_t user_unit_id;

    /// TODO
    int32_t member_type;

    /// TODO
    int32_t disp_order;
};


/// Main object that holds all the player information.
struct UserInfoResp {

    /// ID of the user for this session.
    std::string user_id;

    /// Username shown in-game.
    std::string handle_name;

    /// Account ID used in account transfering.
    std::string account_id;

    /// Password used for account transfering.
    std::string password;

    /// Player ID that is used for the friend system.
    std::string friend_id;

    /// ID of the player used in the ticketing system for player support.
    std::string contact_id;

    /// Get the current tutorial script to execute.
    int32_t tutorial_status;

    /// Checks if the player has finished the tutorial or not.
    bool tutorial_end_flag;

    /// TODO
    std::string user_scenario_info;

    /// TODO
    std::string user_special_scenario_info;

    /// How many times has the user transferred to a new device. May be used to avoid getting first time playing rewards twice.
    int32_t model_change_count;

    /// TODO
    int32_t code_expire_date;

    /// TODO
    int32_t friend_invitation_flag;

    /// Time in seconds until the Gem banner sale ends.
    int64_t early_bird_end;

    /// Enables several debug features (like skipping summoning animation) on the account.
    bool debug_mode;

    /// TODO
    std::string encrypt_iv;

    /// TODO
    std::string encrypted_friend_id;

    /// TODO
    std::string first_desc;

    /// TODO
    std::optional<std::string> dlc_url;

    /// TODO
    std::string feature_gate;

    /// TODO
    std::string unk;
};


/// TODO
struct GumiLiveInfo {

};


///     This object is a container of encrypted JSON data used in DLS API.
/// 
///     The content of the SREE is encrypted with AES/CBC (No padding) and encoded with Base64.
/// 
///     AES Key: 7410958164354871
///     AES IV: Bfw4encrypedPass
struct Sree {

    /// Crypted data
    std::string body;
};


/// Game dynamic configuration
struct GameDls {

    /// Game server address
    std::string game_ip;

    /// CDN server address where resources will be downloaded
    std::string resource_ip;

    /// Game version
    int32_t version;

    /// Gumi live API login server address
    std::string gumilive_ip;

    /// Dynamic background image to show during login
    std::string bg_image;

    /// Block the client login attempt and force it to close
    std::optional<bool> force;

    /// Message to show when the login attempt was blocked (only valid when force is true)
    std::optional<std::string> msg;
};


/// TODO
struct SummonerJournalUserInfo {

    /// TODO
    std::string user_id;

    /// TODO
    int32_t points;

    /// TODO
    int32_t journal_flag;
};


/// TODO
struct DailyLoginRewardsUserInfo {

    /// TODO
    int32_t id;

    /// TODO
    int32_t user_current_count;

    /// TODO
    int32_t user_spin_limit_count;

    /// TODO
    int32_t current_day;

    /// TODO
    int32_t remaining_days_till_guaranteed_reward;

    /// TODO
    std::string message;

    /// TODO
    int32_t next_reward_id;
};


/// TODO
struct GuildInfo {

    /// TODO
    int32_t id;
};


/// TODO
struct ChallengeArenaUserInfo {

    /// TODO
    std::string unk;

    /// TODO
    int32_t unkint;

    /// TODO
    int32_t unkint2;

    /// TODO
    int32_t rainbow_coins;

    /// TODO
    int32_t unkint4;

    /// TODO
    int32_t unkint5;

    /// TODO
    int32_t league_id;

    /// TODO
    int32_t unkint7;

    /// TODO
    int32_t unkint8;

    /// TODO
    int32_t unkint9;

    /// TODO
    std::string unkstr2;
};


/// TODO
struct SignalKey {

    /// TODO
    std::string key;
};


/// TODO
struct DailyTaskPrizeMst {

    /// TODO
    int32_t id;

    /// TODO
    std::string title;

    /// TODO
    std::string desc;

    /// TODO
    int32_t present_type;

    /// TODO
    int32_t reward_id;

    /// TODO
    int32_t reward_count;

    /// TODO
    std::string reward_param;

    /// TODO
    int32_t brave_points_cost;

    /// TODO
    int64_t time_limit;

    /// TODO
    int64_t max_claim_count;

    /// TODO
    int32_t current_claim_count;

    /// TODO
    bool milestone_prize;
};


/// TODO
struct DailyTaskMst {

    /// TODO
    std::string key;

    /// TODO
    std::string title;

    /// TODO
    std::string desc;

    /// TODO
    int32_t task_count;

    /// TODO
    int32_t task_brave_pts;

    /// TODO
    int32_t brave_points_total;

    /// TODO
    int32_t brave_points;

    /// TODO
    std::string area_id;

    /// Number of times the task was completed
    int32_t times_completed;
};


/// TODO
struct DailyTaskBonusMst {

    /// TODO
    int32_t bonus_brave_points;
};


/// TODO
struct NoticeInfo {

    /// TODO
    int32_t id;

    /// TODO
    std::string url;
};


/// TODO
struct ExtraPassiveSkillMst {

    /// TODO
    int32_t id;

    /// TODO
    std::string skill_name;

    /// TODO
    std::string skill_name_s;

    /// TODO
    int32_t group_id;

    /// TODO
    int32_t priority;

    /// TODO
    std::string process_id;

    /// TODO
    std::string process_param;

    /// TODO
    std::string term_param;

    /// TODO
    int32_t target;

    /// TODO
    std::string description;

    /// TODO
    int32_t skill_type;

    /// TODO
    int32_t rare;
};


/// TODO
struct ExcludedDungeonMissionMst {

    /// TODO
    int32_t id;
};


/// Banner configuration in the links page
struct BannerInfoMst {

    /// ID of the banner
    int32_t id;

    /// Internal name of the banner
    std::string name;

    /// List of operative systems allowed
    pkg::string_list<BannerOperativeSystem::Type> target_os;

    /// Order which to display the banner
    int32_t display_order;

    /// TODO
    std::string url;

    /// URL of the image to display
    std::string image;

    /// TODO
    std::string param;

    /// This parameter seems unused in the last version of the game
    std::string page_type;

    /// Number of times the page was read
    int32_t read_count;
};


/// TODO
struct VideoAdsSlotGameInfo {

    /// TODO
    std::string game_info;

    /// TODO
    std::string reel_info;

    /// TODO
    std::string picture_info;

    /// TODO
    std::string game_stand_info;
};


/// TODO
struct VideoAdsSlotGameStandInfo {

    /// TODO
    int32_t ads_count;

    /// TODO
    int32_t max_ads_count;

    /// TODO
    int32_t current_bouns;

    /// TODO
    int32_t max_bouns_count;

    /// TODO
    int32_t ads_bonus_flag;

    /// TODO
    int32_t next_day_timer;
};


/// TODO
struct VideoAdRegion {

    /// TODO
    int32_t id;

    /// TODO
    std::string country_code;
};


/// TODO
struct VideoAdInfo {

    /// TODO
    int32_t id;

    /// TODO
    int32_t available_value;

    /// TODO
    int32_t region_id;

    /// TODO
    bool video_enabled;

    /// TODO
    int64_t next_available_time_left;
};


/// TODO
struct SlotGameReelInfo {

    /// TODO
    int32_t id;

    /// TODO
    std::string reel_data;
};


/// TODO
struct SlotGamePictureInfo {

    /// TODO
    int32_t id;

    /// TODO
    std::string picture_name;
};


/// TODO
struct SlotGameInfo {

    /// TODO
    int32_t id;

    /// TODO
    std::string name;

    /// TODO
    std::string reel_pos;

    /// TODO
    pkg::string_list<std::string> use_medal;

    /// TODO
    std::string slot_help_url;

    /// TODO
    std::string slot_image;
};


/// TODO
struct SlotGameInfoR {

    /// TODO
    SlotGameInfo info;

    /// TODO
    std::vector<SlotGamePictureInfo> pictures;
};


/// TODO
struct NpcMst {

    /// TODO
    int32_t id;

    /// TODO
    std::string handle_name;

    /// TODO
    int32_t arena_rank_id;

    /// TODO
    std::string team;

    /// TODO
    std::string parties;

    /// TODO
    std::string units;
};


/// TODO
struct NpcUnitInfo {

    /// TODO
    int32_t id;

    /// TODO
    int32_t party_id;

    /// TODO
    int32_t type;

    /// TODO
    int32_t lv;

    /// TODO
    int32_t hp;

    /// TODO
    int32_t atk;

    /// TODO
    int32_t deff;

    /// TODO
    int32_t hel;

    /// TODO
    int32_t skill_id;

    /// TODO
    int32_t skill_lv;

    /// TODO
    int32_t equip_item_id;
};


/// TODO
struct NpcTeamInfo {

    /// TODO
    std::string user_id;

    /// TODO
    int32_t lv;

    /// TODO
    std::string friend_message;
};


/// TODO
struct NpcPartyInfo {

    /// TODO
    int32_t id;

    /// TODO
    int32_t type;

    /// TODO
    int32_t disp_order;
};


/// TODO
struct DefineMst {

    /// TODO
    int64_t max_zel;

    /// TODO
    int64_t max_karma;

    /// TODO
    int64_t max_friend_points;

    /// TODO
    int32_t max_team_lv;

    /// TODO
    int32_t max_arena_rank;

    /// TODO
    int32_t max_unit_count;

    /// TODO
    int32_t max_warehouse_count;

    /// TODO
    int32_t default_unit_count;

    /// TODO
    int32_t default_warehouse_count;

    /// TODO
    int32_t friendpoint_rein_friend;

    /// TODO
    pkg::float32 friendpoint_rein_friend_rate;

    /// TODO
    int32_t friendpoint_rein_friend_my;

    /// TODO
    pkg::float32 friendpoint_rein_friend_my_rate;

    /// TODO
    int32_t friendpoint_rein_other;

    /// TODO
    pkg::float32 friendpoint_rein_other_rate;

    /// TODO
    int32_t friendpoint_rein_other_my;

    /// TODO
    pkg::float32 friendpoint_rein_other_my_rate;

    /// TODO
    int32_t permit_invitation;

    /// TODO
    int32_t ext_plus_max_count;

    /// TODO
    int32_t action_point_heal_count;

    /// TODO
    int32_t fight_point_heal_count;

    /// TODO
    int32_t unit_box_ext_count;

    /// TODO
    int32_t item_box_ext_count;

    /// TODO
    int32_t continue_dia_count;

    /// TODO
    std::string initial_unit;

    /// TODO
    int32_t tutorial_dungeon_id;

    /// TODO
    int32_t recover_time_action;

    /// TODO
    int32_t recover_time_fight;

    /// TODO
    int32_t arena_battle_time_limit;

    /// TODO
    int32_t arena_need_mission_id;

    /// TODO
    int32_t max_party_deck_count;

    /// TODO
    std::string arena_tutorial_npc_info;

    /// TODO
    int32_t verify_flag;

    /// TODO
    pkg::float32 unit_mix_great_exp_rate;

    /// TODO
    pkg::float32 unit_mix_super_exp_rate;

    /// TODO
    int32_t recover_time_frohun;

    /// TODO
    int32_t recover_time_raid;

    /// TODO
    int32_t raid_max_p;

    /// TODO
    int32_t raid_bag_count;

    /// TODO
    int32_t friend_ext_count;

    /// TODO
    int32_t medal_max_num;

    /// TODO
    std::string compaign_flag;

    /// TODO
    int32_t max_achieve_point;

    /// TODO
    int32_t zel_per_achieve_point;

    /// TODO
    int32_t karma_per_achieve_point;

    /// TODO
    int32_t max_achieve_point_zel_per_day;

    /// TODO
    int32_t max_achieve_point_karma_per_day;

    /// TODO
    int32_t max_achieve_point_item_per_day;

    /// TODO
    int32_t max_achievement_challenge_accept_count;

    /// TODO
    int32_t max_achievement_record_challenge_accept_count;

    /// TODO
    int32_t tutorial_gatcha_id;

    /// TODO
    std::string parse_overdrive_param;

    /// TODO
    int32_t colosseum_shop_ticket;

    /// TODO
    int32_t max_colosseum_ticket;

    /// TODO
    int32_t max_cbp;

    /// TODO
    int32_t reset_fe_skill_dia_count;

    /// TODO
    int32_t max_blacklist_count;

    /// TODO
    pkg::string_list<int32_t> init_summoner_arm_id;

    /// TODO
    int64_t max_summoner_sp;

    /// TODO
    int64_t max_summoner_friend_point;

    /// TODO
    int32_t max_multi_p;

    /// TODO
    std::string dbb_crystal_values;

    /// TODO
    std::string dbb_fixed_settings_value;

    /// TODO
    std::optional<int32_t> action_point_recover_fixed;

    /// TODO
    std::optional<int32_t> action_point_threshold;
};



/// TODO
struct SummonTicketV2Mst {

};


/// TODO
struct ResummonGachaMst {

};


/// TODO
struct GachaMst {

    /// TODO
    int32_t id;

    /// TODO
    std::string name;

    /// TODO
    int32_t type;

    /// TODO
    int32_t priority;

    /// TODO
    pkg::chrono_time start_date;

    /// TODO
    pkg::chrono_time end_date;

    /// TODO
    std::string start_hour;

    /// TODO
    std::string end_hour;

    /// TODO
    int32_t need_friend_point;

    /// TODO
    int32_t need_gems;

    /// TODO
    bool once_day_flag;

    /// TODO
    std::string bg_img;

    /// TODO
    std::string btn_img;

    /// TODO
    std::string door_img;

    /// TODO
    std::string caption_msg;

    /// TODO
    std::string detail_msg;

    /// TODO
    std::string comment_msg;

    /// TODO
    int32_t gatcha_group_id;

    /// TODO
    std::string description;

    /// TODO
    std::optional<std::string> gatcha_detail_id;

    /// TODO
    std::optional<std::string> contents_banner_img;
};


/// TODO
struct GachaCategory {

    /// TODO
    int32_t id;

    /// TODO
    std::string img;

    /// TODO
    int32_t disp_order;

    /// TODO
    std::string gatcha_id_list;

    /// TODO
    pkg::chrono_time start_date;

    /// TODO
    pkg::chrono_time end_date;
};


/// TODO
struct GachaEffectMst {

    /// TODO
    int32_t id;

    /// TODO
    int32_t gatcha_id;

    /// TODO
    int32_t rare;

    /// TODO
    pkg::float32 rate;

    /// TODO
    std::string effect_before;

    /// TODO
    std::string effect_after;

    /// TODO
    std::string effect;
};


/// TODO
struct ArenaRankMst {

    /// Ranking ID
    int32_t id;

    /// Ranking name
    std::string name;

    /// Points when this rank starts (included)
    int32_t rank_point_start;

    /// Points when this rank ends (included)
    int32_t rank_point_end;

    /// Type of reward
    int32_t reward_type;

    /// Configuration of the reward
    pkg::string_list<int32_t> reward_param;

    /// ID of the scene to play
    std::string scenario_info;
};


/// TODO
struct DungeonKeyMst {

    /// TODO
    int32_t id;

    /// TODO
    std::string name;

    /// TODO
    int32_t dungeon_id;

    /// TODO
    std::string thumbnail_img;

    /// TODO
    std::string open_img;

    /// TODO
    std::string close_img;

    /// TODO
    int32_t limit_sec;

    /// TODO
    std::string possession_limit;

    /// TODO
    std::optional<int32_t> distribute_count;

    /// TODO
    int32_t distribute_flag;

    /// TODO
    std::string usage_pattern;

    /// TODO
    int32_t state;
};


/// TODO
struct TownLocationMst {

    /// TODO
    int32_t id;

    /// TODO
    std::string name;

    /// TODO
    int32_t pos_x;

    /// TODO
    int32_t pos_y;

    /// TODO
    int32_t width;

    /// TODO
    int32_t height;

    /// TODO
    int32_t need_mission_id;

    /// TODO
    int32_t effect_type;
};


/// TODO
struct TownFacilityMst {

    /// TODO
    int32_t id;

    /// TODO
    std::string name;

    /// TODO
    int32_t pos_x;

    /// TODO
    int32_t pos_y;

    /// TODO
    int32_t width;

    /// TODO
    int32_t height;

    /// TODO
    int32_t need_mission_id;
};


/// TODO
struct TownLocationLvMst {

    /// TODO
    int32_t id;

    /// TODO
    int32_t lv;

    /// TODO
    int32_t karma;

    /// TODO
    pkg::string_list<int32_t> release_receipe;
};


/// TODO
struct TownFacilityLvMst {

    /// TODO
    int32_t id;

    /// TODO
    int32_t lv;

    /// TODO
    int32_t karma;

    /// TODO
    pkg::string_list<int32_t> release_receipe;
};


/// TODO
struct UserLevelMst {

    /// TODO
    int32_t level;

    /// TODO
    int64_t exp;

    /// TODO
    int32_t action_points;

    /// TODO
    int32_t deck_cost;

    /// TODO
    int32_t friend_count;

    /// TODO
    int32_t add_friend_count;
};


/// TODO
struct LoginCampaignReward {

    /// TODO
    int32_t id;

    /// TODO
    int32_t reward_day;

    /// TODO
    std::string reward_img;
};


/// TODO
struct LoginCampaignMst {

    /// TODO
    int32_t id;

    /// TODO
    pkg::chrono_time start_date;

    /// TODO
    int32_t total_days;

    /// TODO
    std::string image;
};


/// TODO
struct VersionInfo {

    /// TODO
    std::string id;

    /// TODO
    std::string description;

    /// TODO
    int32_t version;

    /// TODO
    int32_t unknown;

    /// TODO
    int32_t subversion;
};


/// Content of the game request/response.
struct GmeBody {

    /// Encrypted JSON content.
    std::string body;
};


/// Header of a game request/response.
struct GmeHeader {

    /// ID of the request
    std::string id;

    /// ID of the client that invoked the request
    std::string client_id;
};


/// TODO
struct FriendPointInfo {

};


/// TODO
struct FeatureGatingInfo {

};


/// Features of the server
struct FeatureCheck {

    /// TODO
    bool randall;

    /// Enables the frontier hunter
    bool frontier_hunter;

    /// Enables the requirement to use keys for vortex dungeons
    bool field;

    /// TODO
    bool exp_window;

    /// TODO
    bool trial;

    /// Enables sending the Banner MST
    bool send_banner_mst;

    /// TODO
    bool reload_file_mst;

    /// TODO
    bool recommend_app;

    /// Enables the raids
    bool raid;

    /// Enables the beta testing mode of raids
    bool raid_beta;

    /// TODO
    bool dict;

    /// Enables the voices during attacks on units
    bool enable_character_voice;

    /// Allows restarting a raid battle
    bool enable_raid_battle_restart;

    /// Enables auto button in battles
    bool enable_auto_battle;

    /// Enables multisummoning
    bool multi_summon;

    /// TODO
    bool multiaccept;

    /// TODO
    bool facebook_stories;

    /// Enables name changing
    bool enable_name_change;

    /// TODO
    bool randall_facility;

    /// TODO
    bool dailytask_notify;

    /// TODO
    bool dailylogin_gem;

    /// TODO
    bool shop_friend;

    /// TODO
    bool slot;

    /// TODO
    bool sort;

    /// Sets the dungeon keys gave to the players everytime they redeem a key
    int32_t dungeon_key_count_on_redeem;

    /// TODO
    bool dlc_popup_android;

    /// TODO
    bool select_dlc_android;

    /// TODO
    pkg::float32 fps_low;

    /// TODO
    int32_t battle_item_limit;

    /// TODO
    int32_t bb_timer;

    /// TODO
    bool social_special;

    /// TODO
    bool enable_cheats;

    /// TODO
    bool arx_punish_enable;

    /// TODO
    bool arx_popup_enable;

    /// TODO
    bool arx_popup_ios_enable;

    /// TODO
    bool randall_library_memories;

    /// TODO
    bool full_unit_ills_esclude;

    /// Let the cliet checks for corrupted MSTs
    bool check_for_corrupted_mst;

    /// Skips the tutorial
    bool tutorial_skip;

    /// Enables the Grand quest
    bool enable_grand_quests;

    /// Enables achievements in Randall capital
    bool enable_achievements;

    /// TODO
    bool bf_achievement_ext;

    /// Forces the use of Summon tickets before Gems
    bool force_using_summon_tickets;

    /// TODO
    bool feature_gate;

    /// Enables the Challenge Arena
    bool enable_challenge_arena;

    /// TODO
    bool challenge_arena_banner_lock;

    /// TODO
    bool video_ads;

    /// Enables the slot machines with video ads
    bool enable_new_video_ads_slot;

    /// Enables the battle speedup button
    bool enable_battle_speed_button;

    /// Enables the battle speedup button in Challenge Arena
    bool enable_battle_speed_button_in_ca;

    /// Enables the battle speedup button in Arena
    bool enable_battle_speed_button_in_arena;

    /// TODO
    bool autobattle_record;

    /// Enables the colosseum
    bool enable_colosseum;

    /// TODO
    bool sandbag_enable;

    /// TODO
    bool exclude_ca_fusion_unit;

    /// Enables the resummoning banner
    bool enable_resummons;

    /// TODO
    bool sg_target_bundle_flag;

    /// TODO
    bool guild_visible;

    /// Enables guids system
    bool enable_guilds;

    /// TODO
    bool enable_old_video_ads_slot;

    /// TODO
    std::string daily_dungeon_list;

    /// TODO
    bool new_164_trial;

    /// Enables mystery chests
    bool enable_mystery_chests;

    /// TODO
    bool freepaid_gems;

    /// TODO
    int32_t cooldown_timer;

    /// TODO
    bool va_sp_skill;

    /// TODO
    bool frontiergate_plus;

    /// Enables category in summons
    bool enable_summon_categories;

    /// TODO
    bool unit_type_bonus_skill;

    /// TODO
    bool fish;

    /// TODO
    bool felloplay_community;

    /// TODO
    bool supersonic_google;

    /// TODO
    bool google_felloplay;

    /// TODO
    bool felloplay_community_ios;

    /// TODO
    bool amazon_coins_reward_control;
};


/// TODO
struct BadgeInfo {

    /// TODO
    int32_t scenario_num;

    /// TODO
    int32_t unit_dict_num;

    /// TODO
    int32_t item_dict_num;

    /// TODO
    int32_t new_frohun;

    /// TODO
    int32_t dungeon_key_num;

    /// TODO
    std::string badge_data;
};


/// TODO
struct UrlMst {

    /// TODO
    std::string id;

    /// TODO
    std::string official_site;

    /// TODO
    std::string notice_url;

    /// TODO
    std::string contact_url;

    /// TODO
    std::string friend_refeer_url;

    /// TODO
    std::string appli_dl_url;

    /// TODO
    std::string appli_dl_android_url;

    /// TODO
    std::string fami_app_site_url;

    /// TODO
    std::string facebook_site_url;

    /// TODO
    std::string twitter_site_url;

    /// TODO
    std::string transfer_site_url;

    /// TODO
    std::string app_bank_site_url;

    /// TODO
    std::string lobl_site_url;

    /// TODO
    std::string lobl_schema_url;

    /// TODO
    std::string appli_sommelier_url;

    /// TODO
    std::string credit_url;

    /// TODO
    std::string game_gift_url;

    /// TODO
    std::string agreement_url;

    /// TODO
    std::string agreement_official_url;

    /// TODO
    std::string legal_fund_settlement_url;

    /// TODO
    std::string specific_trade_url;

    /// TODO
    std::string dia_possession_url;

    /// TODO
    std::string lobi_rec_help_url;

    /// TODO
    std::string lobi_agreement_url;

    /// TODO
    std::string gacha_contents_url;

    /// TODO
    std::string multi_archive_url;
};


/// F_UNIT_MST
struct UnitMst {

    /// TODO
    int32_t id;

    /// TODO
    std::string unit_name;

    /// TODO
    int32_t rarity;

    /// TODO
    int32_t cost;

    /// TODO
    int32_t kind;

    /// TODO
    int32_t max_lv;

    /// TODO
    int32_t min_hp;

    /// TODO
    int32_t max_hp;

    /// TODO
    int32_t min_atk;

    /// TODO
    int32_t max_atk;

    /// TODO
    int32_t min_def;

    /// TODO
    int32_t max_def;

    /// TODO
    int32_t min_rec;

    /// TODO
    int32_t max_rec;

    /// TODO
    std::vector<std::string> param_max;

    /// TODO
    int32_t arousal;

    /// TODO
    int32_t element;

    /// TODO
    int32_t sex;

    /// TODO
    int32_t tribe;

    /// TODO
    std::string effect_frame;

    /// TODO
    std::string damage_frame;

    /// TODO
    int32_t drop_check_count;

    /// TODO
    int32_t leader_skill_id;

    /// TODO
    int32_t skill_id;

    /// TODO
    int32_t extra_skill_id;

    /// TODO
    int32_t ultimate_skill_id;

    /// TODO
    std::vector<int32_t> unknown;

    /// TODO
    int32_t extra_passive_skill_id;

    /// TODO
    std::string anim_cgg;

    /// TODO
    int32_t sell_price;

    /// TODO
    bool caution_type;

    /// TODO
    int32_t description_type;

    /// TODO
    int32_t move_speed;

    /// TODO
    int32_t attack_move_type;

    /// TODO
    int32_t back_move_type;

    /// TODO
    int32_t skill_move_type;

    /// TODO
    int32_t ai_id;

    /// TODO
    int32_t exp_pattern_id;

    /// TODO
    pkg::string_list<int32_t> bad_state_resists;

    /// TODO
    int32_t adjust_exp;

    /// TODO
    pkg::string_list<int32_t> home_img_pos;

    /// TODO
    pkg::string_list<int32_t> detail_img_pos;

    /// TODO
    pkg::string_list<int32_t> confirm_img_pos;

    /// TODO
    pkg::string_list<int32_t> summon_image_pos;

    /// TODO
    pkg::string_list<int32_t> skill_cut_image_pos;

    /// TODO
    pkg::string_list<int32_t> hp_disp_pos;

    /// TODO
    int32_t disp_order;

    /// TODO
    pkg::string_list<int32_t> move_offset;

    /// TODO
    int32_t category;

    /// TODO
    int32_t skill_up_adjust;

    /// TODO
    pkg::string_list<int32_t> character_voice;

    /// TODO
    int32_t getting_type;

    /// TODO
    std::string ext_home_img_pos;

    /// TODO
    std::string ext_detail_img_pos;

    /// TODO
    std::string ext_confirm_img_pos;

    /// TODO
    std::string ext_summon_img_pos;

    /// TODO
    std::string ext_skill_cutin_img_pos;

    /// TODO
    pkg::string_list<int32_t> cursor_disp_pos;

    /// TODO
    bool disp_dict_flag;
};


/// TODO
struct UnitExpPatternMst {

    /// TODO
    int32_t id;

    /// TODO
    int32_t lv;

    /// TODO
    int32_t need_exp;
};


/// Multiline document string.
/// 
/// # Example
struct ReceipeMst {

    /// TODO
    int32_t id;

    /// TODO
    int32_t item_id;

    /// TODO
    int32_t item_count;

    /// TODO
    std::vector<std::string> materials;

    /// TODO
    int32_t karma;

    /// TODO
    std::string keyitem_ids;

    /// TODO
    std::string unk;

    /// TODO
    int32_t unk2;

    /// TODO
    pkg::chrono_time unk3;

    /// TODO
    pkg::chrono_time unk4;
};


/// TODO
struct GiftItemMst {

};


/// TODO
struct FirstDescMst {

};


/// TODO
struct GeneralEventMst {

};


/// TODO
struct UserInfoReq {

    /// Username shown in-game.
    std::string handle_name;

    /// TODO
    std::string user_id;

    /// TODO
    std::string contact_id;

    /// TODO
    int32_t model_change_count;

    /// TODO
    std::string device_name;

    /// Operative system which runs the game
    GameTargetOs::Type target_os;

    /// TODO
    std::string build_platform_id;

    /// Device advertisement ID (the same as vid in the gumi api login
    std::string device_id;

    /// TODO
    std::string pointer_name;

    /// TODO
    std::string first_desc_mst_req;

    /// TODO
    std::string notice_mst_list_req;

    /// TODO
    std::string minfo;

    /// Current game language
    GameLanguages::Type current_language;

    /// Country code
    std::string country_code;

    /// Gumi live API User ID. (used for example in IAP)
    std::string gumi_live_userid;

    /// User token of the Gumi live API.
    std::string gumi_live_token;
};


/// Initialization command
struct InitializeReq {

    /// User information request
    UserInfoReq user_info;

    /// MST that should be sent
    std::vector<MstUrlList> mst_requests;
};


/// Gumi API login request
struct GuestLoginReq {

    /// Device model
    std::string device_model;

    /// Device platform
    PlatformEnum::Type device_platform;

    /// Device Advertising ID (this can be either the Google Play ID or the Microsoft Ad ID)
    std::string device_ad_id;

    /// Device version
    std::string device_version;

    /// A random alternative UUID to identify the device
    std::string alt_vid;

    /// TODO
    std::string ak;

    /// TODO
    std::string identifiers;
};


/// Gumi API login
struct GuestLogin {

    /// Status of the operation
    StatusEnum::Type status;

    /// Login token
    std::string token;

    /// ID of the game to perform the login
    std::string user_id;

    /// ID of the error
    int32_t status_number;
};

/// TODO
enum class RewardType {

    /// TODO
    Type0 = 0,

    /// TODO
    Type1,

    /// TODO
    Type2
};

/// Type of command to do after the user presses OK.
enum class GmeErrorCommand {

    /// TODO
    Retry = 2,

    /// TODO
    Continue = 3,

    /// TODO
    Close = 4,

    /// TODO
    Close2 = 5,

    /// TODO
    ReturnToGame = 6,

    /// TODO
    RaidError = 7,

    /// TODO
    Continue2 = 8,

    /// TODO
    LogoutFacebook = 9,

    /// TODO
    Close3 = 10
};

/// Flags of the error happend in the game.
enum class GmeErrorFlags {

    /// No error oncurred.
    NoError = 0,

    /// The server oncurred an error.
    IsInError = 1,

    /// The custom command should be handled.
    ShouldHandleCommands = 2
};


/// Object that stores any possible error with the request or response.
struct GmeError {

    /// Error flags.
    GmeErrorFlags flag;

    /// Action to execute after pressing OK. (Only enabled if the flag `ShouldHandleCommands` is set)
    GmeErrorCommand cmd;

    /// Message to show on the error.
    std::string message;

    /// URL to open in the browser after OK is pressed. (like for update the game)
    std::string url;
};


/// Main packet of interaction between client and server.
struct GmeAction {

    /// Header of the message.
    GmeHeader header;

    /// Body of the message.
    std::optional<GmeBody> body;

    /// Error object in case of an error.
    std::optional<GmeError> error;
};


/// Response of the initialize command
struct InitializeResp {

    /// Configuration about the current login progression
    LoginCampaignMst login_campagin;

    /// Configuration about each day of the login campaign
    std::vector<LoginCampaignReward> login_campaign_reward;

    /// Configuration of the user stats progression
    std::vector<UserLevelMst> progression;

    /// Configuration of the versions of the MSTs
    std::vector<VersionInfo> mst;

    /// TODO
    std::vector<TownFacilityMst> town_facility;

    /// TODO
    std::vector<TownFacilityLvMst> town_facility_lv;

    /// TODO
    std::vector<TownLocationMst> town_location;

    /// TODO
    std::vector<TownLocationLvMst> town_location_lv;

    /// Configuration of the Vortex dungeon with keys
    std::vector<DungeonKeyMst> dungeon_keys;

    /// PVP Arena rankings
    std::vector<ArenaRankMst> arena_ranks;

    /// TODO
    std::vector<GachaEffectMst> gacha_effects;

    /// Configuration of current Gacha banners
    std::vector<GachaMst> gachas;

    /// Configuration of the server
    DefineMst defines;

    /// TODO
    std::vector<NpcMst> npcs;

    /// TODO
    std::vector<BannerInfoMst> banner_info;

    /// TODO
    std::vector<ExtraPassiveSkillMst> extra_passive_skills;

    /// Configuration of notices
    NoticeInfo notice_info;

    /// TODO
    SignalKey signal_key;

    /// User current progression in Challenge Arena
    ChallengeArenaUserInfo challenge_arena_user_info;

    /// TODO
    DailyTaskBonusMst daily_task_bonuses;

    /// TODO
    std::vector<DailyTaskPrizeMst> daily_task_prizes;

    /// TODO
    std::vector<DailyTaskMst> daily_tasks;

    /// TODO
    DailyLoginRewardsUserInfo daily_login_rewards;

    /// Guild information
    GuildInfo guild;

    /// TODO
    VideoAdsSlotGameInfo video_ad_slots;

    /// TODO
    SummonerJournalUserInfo summoner_journal;

    /// User information
    UserInfoResp user_info;
};

// Auto-generated Glaze definitions
#if __has_include(<glaze/glaze.hpp>)
#include <pkgen_glaze_helpers.hpp>

template <>
struct glz::meta<GeneralEventMst> {
    using T = GeneralEventMst;
    static constexpr auto value = object(

    );
};

template <>
struct glz::meta<FirstDescMst> {
    using T = FirstDescMst;
    static constexpr auto value = object(

    );
};

template <>
struct glz::meta<GiftItemMst> {
    using T = GiftItemMst;
    static constexpr auto value = object(

    );
};

template <>
struct glz::meta<ReceipeMst> {
    using T = ReceipeMst;
    static constexpr auto value = object(
        "4HqhTf3a", glz::quoted_num<&T::id>,
        "kixHbe54", &T::item_id,
        "wgV86x1q", &T::item_count,
        "75YV2q1i", &T::materials,
        "HTVh8a65", &T::karma,
        "7dEB1Kwj", &T::keyitem_ids,
        "XuJL4pc5", &T::unk,
        "H6k1LIxC", &T::unk2,
        "qA7M9EjP", pkg::glaze::datetime<&T::unk3>(),
        "SzV0Nps7", pkg::glaze::datetime<&T::unk4>()
    );
};

template <>
struct glz::meta<UnitExpPatternMst> {
    using T = UnitExpPatternMst;
    static constexpr auto value = object(
        "5UvTp7q1", &T::id,
        "D9wXQI2V", &T::lv,
        "grKTq15A", &T::need_exp
    );
};

template <>
struct glz::meta<UnitMst> {
    using T = UnitMst;
    static constexpr auto value = object(
        "pn16CNah", &T::id,
        "utP1c0CD", &T::unit_name,
        "7ofj5xa1", &T::rarity,
        "gvT2ds0Q", &T::cost,
        "xP9oi5Z2", &T::kind,
        "EI1DF8Yt", &T::max_lv,
        "UZ1Bj7w2", &T::min_hp,
        "3WMz78t6", &T::max_hp,
        "i9Tn7kYr", &T::min_atk,
        "omuyP54D", &T::max_atk,
        "q78KoWsg", &T::min_def,
        "32INDST4", &T::max_def,
        "92ij6UGB", &T::min_rec,
        "X9P3AN5d", &T::max_rec,
        "imQJdg64", &T::param_max,
        "2SH6YsJi", &T::arousal,
        "iNy0ZU5M", &T::element,
        "9i2xhMaJ", &T::sex,
        "07nTrLdD", &T::tribe,
        "eyUo6a8c", &T::effect_frame,
        "6Aou5M9r", &T::damage_frame,
        "n9h7p02P", &T::drop_check_count,
        "oS3kTZ2W", &T::leader_skill_id,
        "nj9Lw7mV", &T::skill_id,
        "iEFZ6H19", &T::extra_skill_id,
        "cb0P4mp1", &T::ultimate_skill_id,
        "5SUvj4tM", &T::unknown,
        "cP83zNsv", &T::extra_passive_skill_id,
        "QqfI9mM4", &T::anim_cgg,
        "eKtE6k0n", &T::sell_price,
        "y4jAZ9nI", pkg::glaze::bool_as_string<&T::caution_type>(),
        "qp37xTDh", &T::description_type,
        "J2hPXGo5", &T::move_speed,
        "mv4o39Uz", &T::attack_move_type,
        "h2L1YI90", &T::back_move_type,
        "XE7Yi5c3", &T::skill_move_type,
        "i74vGUFa", &T::ai_id,
        "5UvTp7q1", &T::exp_pattern_id,
        "CEeqs63b", pkg::glaze::array_string<T, &T::bad_state_resists, ':'>,
        "Gd4NS7H6", &T::adjust_exp,
        "1W9CxaFK", pkg::glaze::array_string<T, &T::home_img_pos, ','>,
        "6z54rgb3", pkg::glaze::array_string<T, &T::detail_img_pos, ','>,
        "MYK1fq6c", pkg::glaze::array_string<T, &T::confirm_img_pos, ','>,
        "KC3Jk8Br", pkg::glaze::array_string<T, &T::summon_image_pos, ','>,
        "7hLR6pDN", pkg::glaze::array_string<T, &T::skill_cut_image_pos, ','>,
        "3BpHN6VD", pkg::glaze::array_string<T, &T::hp_disp_pos, ','>,
        "XuJL4pc5", &T::disp_order,
        "jFX0a7qe", pkg::glaze::array_string<T, &T::move_offset, ','>,
        "9PsmH7tz", &T::category,
        "PXD4v2KY", &T::skill_up_adjust,
        "QwN4hqJ2", pkg::glaze::array_string<T, &T::character_voice, ','>,
        "6yHMXYv1", &T::getting_type,
        "jyj6bl9P", &T::ext_home_img_pos,
        "QHhOLCuk", &T::ext_detail_img_pos,
        "S0BjxW1F", &T::ext_confirm_img_pos,
        "psOjpTAv", &T::ext_summon_img_pos,
        "9y1vaEJk", &T::ext_skill_cutin_img_pos,
        "Lkh6gYkT", pkg::glaze::array_string<T, &T::cursor_disp_pos, ','>,
        "S4ezp6uK", pkg::glaze::bool_as_string<&T::disp_dict_flag>()
    );
};

template <>
struct glz::meta<UrlMst> {
    using T = UrlMst;
    static constexpr auto value = object(
        "ut3nd0yV", &T::id,
        "ME4h3vDn", &T::official_site,
        "cH0Kdchp", &T::notice_url,
        "UMRh8tS2", &T::contact_url,
        "R4Dk2FYs", &T::friend_refeer_url,
        "E1oFw94b", &T::appli_dl_url,
        "D0gMyJ3o", &T::appli_dl_android_url,
        "UWb9B6sm", &T::fami_app_site_url,
        "HcIt7G8y", &T::facebook_site_url,
        "13kI62fP", &T::twitter_site_url,
        "Rf63vLu7", &T::transfer_site_url,
        "e4qEPM8j", &T::app_bank_site_url,
        "t9jT3QKC", &T::lobl_site_url,
        "Yh6MiF8f", &T::lobl_schema_url,
        "25oCPAM7", &T::appli_sommelier_url,
        "iVgm9n6v", &T::credit_url,
        "wT6cF35V", &T::game_gift_url,
        "3f9T5d6G", &T::agreement_url,
        "fNud6c7r", &T::agreement_official_url,
        "z6q1aUXE", &T::legal_fund_settlement_url,
        "AZ2mCbU1", &T::specific_trade_url,
        "9Nrt0SPY", &T::dia_possession_url,
        "tK24myuj", &T::lobi_rec_help_url,
        "6Qj8YZtE", &T::lobi_agreement_url,
        "qlO1kVZZ", &T::gacha_contents_url,
        "R10T3gFJ", &T::multi_archive_url
    );
};

template <>
struct glz::meta<BadgeInfo> {
    using T = BadgeInfo;
    static constexpr auto value = object(
        "u7Htm9P4", glz::quoted_num<&T::scenario_num>,
        "yCLEs28F", glz::quoted_num<&T::unit_dict_num>,
        "iakSd6m8", glz::quoted_num<&T::item_dict_num>,
        "4AjsNu0m", glz::quoted_num<&T::new_frohun>,
        "dD40xrsh", glz::quoted_num<&T::dungeon_key_num>,
        "dUujjBBK", &T::badge_data
    );
};

template <>
struct glz::meta<FeatureCheck> {
    using T = FeatureCheck;
    static constexpr auto value = object(
        "randall", glz::bools_as_numbers<&T::randall>,
        "frontierhunter", glz::bools_as_numbers<&T::frontier_hunter>,
        "dungeon_key", glz::bools_as_numbers<&T::field>,
        "exp_window", glz::bools_as_numbers<&T::exp_window>,
        "trial", glz::bools_as_numbers<&T::trial>,
        "banner_mst", glz::bools_as_numbers<&T::send_banner_mst>,
        "reload_file_mst", glz::bools_as_numbers<&T::reload_file_mst>,
        "recommend_app", glz::bools_as_numbers<&T::recommend_app>,
        "raid", glz::bools_as_numbers<&T::raid>,
        "raid_beta", glz::bools_as_numbers<&T::raid_beta>,
        "dict", glz::bools_as_numbers<&T::dict>,
        "character_voice", glz::bools_as_numbers<&T::enable_character_voice>,
        "raid_battle_restart", glz::bools_as_numbers<&T::enable_raid_battle_restart>,
        "autobattle", glz::bools_as_numbers<&T::enable_auto_battle>,
        "multisummon", glz::bools_as_numbers<&T::multi_summon>,
        "multiaccept", glz::bools_as_numbers<&T::multiaccept>,
        "facebook_stories", glz::bools_as_numbers<&T::facebook_stories>,
        "name_change_func", glz::bools_as_numbers<&T::enable_name_change>,
        "randall_facility", glz::bools_as_numbers<&T::randall_facility>,
        "dailytask_notify", glz::bools_as_numbers<&T::dailytask_notify>,
        "dailylogin_gem", glz::bools_as_numbers<&T::dailylogin_gem>,
        "shop_friend", glz::bools_as_numbers<&T::shop_friend>,
        "slot", glz::bools_as_numbers<&T::slot>,
        "sort", glz::bools_as_numbers<&T::sort>,
        "dungeon_key_cnt", &T::dungeon_key_count_on_redeem,
        "dlc_popup_android", glz::bools_as_numbers<&T::dlc_popup_android>,
        "select_dlc_android", glz::bools_as_numbers<&T::select_dlc_android>,
        "fps_low", glz_write_f32(&T::fps_low),
        "battle_item_limit", &T::battle_item_limit,
        "bb_timer", &T::bb_timer,
        "social_special", glz::bools_as_numbers<&T::social_special>,
        "ischeat_enable", glz::bools_as_numbers<&T::enable_cheats>,
        "arx_punish_enable", glz::bools_as_numbers<&T::arx_punish_enable>,
        "arx_popup_enable", glz::bools_as_numbers<&T::arx_popup_enable>,
        "arx_popup_ios_enable", glz::bools_as_numbers<&T::arx_popup_ios_enable>,
        "randall_library_memories", glz::bools_as_numbers<&T::randall_library_memories>,
        "full_unit_ills_esclude", glz::bools_as_numbers<&T::full_unit_ills_esclude>,
        "corrupted_mst_check", glz::bools_as_numbers<&T::check_for_corrupted_mst>,
        "tutorial_skip", glz::bools_as_numbers<&T::tutorial_skip>,
        "bf_campaign_grand_quest", glz::bools_as_numbers<&T::enable_grand_quests>,
        "bf_achievement", glz::bools_as_numbers<&T::enable_achievements>,
        "bf_achievement_ext", glz::bools_as_numbers<&T::bf_achievement_ext>,
        "force_use_summon_tickets", glz::bools_as_numbers<&T::force_using_summon_tickets>,
        "feature_gate", glz::bools_as_numbers<&T::feature_gate>,
        "challenge_arena", glz::bools_as_numbers<&T::enable_challenge_arena>,
        "challenge_arena_banner_lock", glz::bools_as_numbers<&T::challenge_arena_banner_lock>,
        "video_ads", glz::bools_as_numbers<&T::video_ads>,
        "video_ads_slot", glz::bools_as_numbers<&T::enable_new_video_ads_slot>,
        "battle_speed", glz::bools_as_numbers<&T::enable_battle_speed_button>,
        "battle_speed_ca", glz::bools_as_numbers<&T::enable_battle_speed_button_in_ca>,
        "battle_speed_arena_pvp", glz::bools_as_numbers<&T::enable_battle_speed_button_in_arena>,
        "autobattle_record", glz::bools_as_numbers<&T::autobattle_record>,
        "colosseum_enable", glz::bools_as_numbers<&T::enable_colosseum>,
        "sandbag_enable", glz::bools_as_numbers<&T::sandbag_enable>,
        "exclude_ca_fusion_unit", glz::bools_as_numbers<&T::exclude_ca_fusion_unit>,
        "sg_resummon_gacha_enable", glz::bools_as_numbers<&T::enable_resummons>,
        "sg_target_bundle_flag", glz::bools_as_numbers<&T::sg_target_bundle_flag>,
        "guild_visible", glz::bools_as_numbers<&T::guild_visible>,
        "guild", glz::bools_as_numbers<&T::enable_guilds>,
        "old_video_ads_slot", glz::bools_as_numbers<&T::enable_old_video_ads_slot>,
        "daily_dungeon_list", &T::daily_dungeon_list,
        "new_164_trial", glz::bools_as_numbers<&T::new_164_trial>,
        "mystery_chest", glz::bools_as_numbers<&T::enable_mystery_chests>,
        "freepaid_gems", glz::bools_as_numbers<&T::freepaid_gems>,
        "cooldown_timer", &T::cooldown_timer,
        "va_sp_skill", glz::bools_as_numbers<&T::va_sp_skill>,
        "frontiergate_plus", glz::bools_as_numbers<&T::frontiergate_plus>,
        "gacha_category", glz::bools_as_numbers<&T::enable_summon_categories>,
        "unit_type_bonus_skill", glz::bools_as_numbers<&T::unit_type_bonus_skill>,
        "fish", glz::bools_as_numbers<&T::fish>,
        "felloplay_community", glz::bools_as_numbers<&T::felloplay_community>,
        "supersonic_google", glz::bools_as_numbers<&T::supersonic_google>,
        "google_felloplay", glz::bools_as_numbers<&T::google_felloplay>,
        "felloplay_community_ios", glz::bools_as_numbers<&T::felloplay_community_ios>,
        "amazon_conins_reward_control", glz::bools_as_numbers<&T::amazon_coins_reward_control>
    );
};

template <>
struct glz::meta<FeatureGatingInfo> {
    using T = FeatureGatingInfo;
    static constexpr auto value = object(

    );
};

template <>
struct glz::meta<FriendPointInfo> {
    using T = FriendPointInfo;
    static constexpr auto value = object(

    );
};

template <>
struct glz::meta<GmeHeader> {
    using T = GmeHeader;
    static constexpr auto value = object(
        "Hhgi79M1", &T::id,
        "aV6cLn3v", &T::client_id
    );
};

template <>
struct glz::meta<GmeBody> {
    using T = GmeBody;
    static constexpr auto value = object(
        "Kn51uR4Y", &T::body
    );
};

template <>
struct glz::meta<GmeError> {
    using T = GmeError;
    static constexpr auto value = object(
        "3e9aGpus", &T::flag,
        "iPD12YCr", &T::cmd,
        "ZC0msu2L", &T::message,
        "zcJeTx18", &T::url
    );
};

template <>
struct glz::meta<GmeAction> {
    using T = GmeAction;
    static constexpr auto value = object(
        "F4q6i9xe", &T::header,
        "a3vSYuq2", &T::body,
        "b5PH6mZa", &T::error
    );
};

template <>
struct glz::meta<VersionInfo> {
    using T = VersionInfo;
    static constexpr auto value = object(
        "moWQ30GH", &T::id,
        "e3QNsuZ8", &T::description,
        "d2RFtP8T", glz::quoted_num<&T::version>,
        "H6k1LIxC", glz::quoted_num<&T::unknown>,
        "5kbnkTp0", glz::quoted_num<&T::subversion>
    );
};

template <>
struct glz::meta<LoginCampaignMst> {
    using T = LoginCampaignMst;
    static constexpr auto value = object(
        "H1Dkq93v", glz::quoted_num<&T::id>,
        "qA7M9EjP", pkg::glaze::datetime_unix<&T::start_date>(),
        "1adb38d5", glz::quoted_num<&T::total_days>,
        "b38adb8i", &T::image
    );
};

template <>
struct glz::meta<LoginCampaignReward> {
    using T = LoginCampaignReward;
    static constexpr auto value = object(
        "H1Dkq93v", glz::quoted_num<&T::id>,
        "n0He37p1", glz::quoted_num<&T::reward_day>,
        "b38adb8i", &T::reward_img
    );
};

template <>
struct glz::meta<UserLevelMst> {
    using T = UserLevelMst;
    static constexpr auto value = object(
        "D9wXQI2V", glz::quoted_num<&T::level>,
        "d96tuT2E", glz::quoted_num<&T::exp>,
        "0P9X1YHs", glz::quoted_num<&T::action_points>,
        "ziex06DY", glz::quoted_num<&T::deck_cost>,
        "oFQ3mbS6", glz::quoted_num<&T::friend_count>,
        "GZ2rKW90", glz::quoted_num<&T::add_friend_count>
    );
};

template <>
struct glz::meta<TownFacilityLvMst> {
    using T = TownFacilityLvMst;
    static constexpr auto value = object(
        "y9ET7Aub", glz::quoted_num<&T::id>,
        "D9wXQI2V", glz::quoted_num<&T::lv>,
        "HTVh8a65", glz::quoted_num<&T::karma>,
        "rGoJ6Ty9", pkg::glaze::array_string<T, &T::release_receipe, ','>
    );
};

template <>
struct glz::meta<TownLocationLvMst> {
    using T = TownLocationLvMst;
    static constexpr auto value = object(
        "y9ET7Aub", glz::quoted_num<&T::id>,
        "D9wXQI2V", glz::quoted_num<&T::lv>,
        "HTVh8a65", glz::quoted_num<&T::karma>,
        "rGoJ6Ty9", pkg::glaze::array_string<T, &T::release_receipe, ','>
    );
};

template <>
struct glz::meta<TownFacilityMst> {
    using T = TownFacilityMst;
    static constexpr auto value = object(
        "y9ET7Aub", glz::quoted_num<&T::id>,
        "aAFI6S5w", &T::name,
        "SnNtTh51", glz::quoted_num<&T::pos_x>,
        "M6C1aXfR", glz::quoted_num<&T::pos_y>,
        "dRhvW13q", glz::quoted_num<&T::width>,
        "FCzW4g6P", glz::quoted_num<&T::height>,
        "HSRhkf70", &T::need_mission_id
    );
};

template <>
struct glz::meta<TownLocationMst> {
    using T = TownLocationMst;
    static constexpr auto value = object(
        "un80kW9Y", glz::quoted_num<&T::id>,
        "6ntp4rMV", &T::name,
        "SnNtTh51", glz::quoted_num<&T::pos_x>,
        "M6C1aXfR", glz::quoted_num<&T::pos_y>,
        "dRhvW13q", glz::quoted_num<&T::width>,
        "FCzW4g6P", glz::quoted_num<&T::height>,
        "HSRhkf70", &T::need_mission_id,
        "jeR2rN3V", glz::quoted_num<&T::effect_type>
    );
};

template <>
struct glz::meta<DungeonKeyMst> {
    using T = DungeonKeyMst;
    static constexpr auto value = object(
        "16KMNJLb", glz::quoted_num<&T::id>,
        "BM29ZgnK", &T::name,
        "MHx05sXt", glz::quoted_num<&T::dungeon_id>,
        "M2cv6dum", &T::thumbnail_img,
        "VX0j1fni", &T::open_img,
        "9unNZ6b0", &T::close_img,
        "i9sBW8uD", glz::quoted_num<&T::limit_sec>,
        "N7I9vYZb", &T::possession_limit,
        "khsb74Nq", glz::quoted_num<&T::distribute_count>,
        "EK5I6MQ9", glz::quoted_num<&T::distribute_flag>,
        "CR6aKWg8", &T::usage_pattern,
        "j0Uszek2", glz::quoted_num<&T::state>
    );
};

template <>
struct glz::meta<ArenaRankMst> {
    using T = ArenaRankMst;
    static constexpr auto value = object(
        "JmFn3g9t", glz::quoted_num<&T::id>,
        "rGm09bav", &T::name,
        "w0aTd94Y", glz::quoted_num<&T::rank_point_start>,
        "1U3eBCyY", glz::quoted_num<&T::rank_point_end>,
        "IkmC8gG2", glz::quoted_num<&T::reward_type>,
        "empaR60j", pkg::glaze::array_string<T, &T::reward_param, ':'>,
        "N4XVE1uA", &T::scenario_info
    );
};

template <>
struct glz::meta<GachaEffectMst> {
    using T = GachaEffectMst;
    static constexpr auto value = object(
        "u0vkt9yH", glz::quoted_num<&T::id>,
        "7Ffmi96v", glz::quoted_num<&T::gatcha_id>,
        "7ofj5xa1", glz::quoted_num<&T::rare>,
        "ug9xV4Fz", glz::quoted_num<&T::rate>,
        "7ZNcmYS2", &T::effect_before,
        "tj0i9JhC", &T::effect_after,
        "YTx3c1jQ", &T::effect
    );
};

template <>
struct glz::meta<GachaCategory> {
    using T = GachaCategory;
    static constexpr auto value = object(
        "vx9uyQVQ", &T::id,
        "In7lGGLn", &T::img,
        "2r4EoNt4", &T::disp_order,
        "3rCmq58M", &T::gatcha_id_list,
        "qA7M9EjP", pkg::glaze::datetime_unix<&T::start_date>(),
        "SzV0Nps7", pkg::glaze::datetime_unix<&T::end_date>()
    );
};

template <>
struct glz::meta<GachaMst> {
    using T = GachaMst;
    static constexpr auto value = object(
        "7Ffmi96v", glz::quoted_num<&T::id>,
        "4N27mkt1", &T::name,
        "S1oz60Hc", glz::quoted_num<&T::type>,
        "yu18xScw", glz::quoted_num<&T::priority>,
        "qA7M9EjP", pkg::glaze::datetime<&T::start_date>(),
        "SzV0Nps7", pkg::glaze::datetime<&T::end_date>(),
        "2HY3jpgu", &T::start_hour,
        "v9TR3cDz", &T::end_hour,
        "J3stQ7jd", glz::quoted_num<&T::need_friend_point>,
        "03UGMHxF", glz::quoted_num<&T::need_gems>,
        "4tswNoV9", pkg::glaze::bool_as_string<&T::once_day_flag>(),
        "1Dg0vUX3", &T::bg_img,
        "W9ABuJj2", &T::btn_img,
        "uKYf13AH", &T::door_img,
        "3sdHQb69", &T::caption_msg,
        "W2c9g0Je", &T::detail_msg,
        "gVSj32QH", &T::comment_msg,
        "TCnm1F4v", glz::quoted_num<&T::gatcha_group_id>,
        "qp37xTDh", &T::description,
        "8Z9CYQDq", &T::gatcha_detail_id,
        "sA9dDAqB", &T::contents_banner_img
    );
};

template <>
struct glz::meta<ResummonGachaMst> {
    using T = ResummonGachaMst;
    static constexpr auto value = object(

    );
};

template <>
struct glz::meta<SummonTicketV2Mst> {
    using T = SummonTicketV2Mst;
    static constexpr auto value = object(

    );
};

template <>
struct glz::meta<DefineMst> {
    using T = DefineMst;
    static constexpr auto value = object(
        "DXm2W8vY", glz::quoted_num<&T::max_zel>,
        "jSxdlwqd", glz::quoted_num<&T::max_karma>,
        "4YCzox9t", glz::quoted_num<&T::max_friend_points>,
        "Kt8H4LN7", glz::quoted_num<&T::max_team_lv>,
        "tzCfGX83", glz::quoted_num<&T::max_arena_rank>,
        "ouXxIY63", glz::quoted_num<&T::max_unit_count>,
        "5pjoGBC4", glz::quoted_num<&T::max_warehouse_count>,
        "k0xrd38b", glz::quoted_num<&T::default_unit_count>,
        "8U93zxsf", glz::quoted_num<&T::default_warehouse_count>,
        "2oD1fmuX", glz::quoted_num<&T::friendpoint_rein_friend>,
        "PnE6xo93", glz::quoted_num<&T::friendpoint_rein_friend_rate>,
        "86sx4FfJ", glz::quoted_num<&T::friendpoint_rein_friend_my>,
        "JBR0Po3b", glz::quoted_num<&T::friendpoint_rein_friend_my_rate>,
        "K10QSeuj", glz::quoted_num<&T::friendpoint_rein_other>,
        "6p4YW7oc", glz::quoted_num<&T::friendpoint_rein_other_rate>,
        "VkA3nu0b", glz::quoted_num<&T::friendpoint_rein_other_my>,
        "mI1jW0X7", glz::quoted_num<&T::friendpoint_rein_other_my_rate>,
        "02IgM6ib", glz::quoted_num<&T::permit_invitation>,
        "4t3qX2kT", glz::quoted_num<&T::ext_plus_max_count>,
        "C8KkHGa7", glz::quoted_num<&T::action_point_heal_count>,
        "sy9G24Su", glz::quoted_num<&T::fight_point_heal_count>,
        "I73XkAQi", glz::quoted_num<&T::unit_box_ext_count>,
        "CA01vo2Q", glz::quoted_num<&T::item_box_ext_count>,
        "QW3HiNv8", glz::quoted_num<&T::continue_dia_count>,
        "21ovwqYT", &T::initial_unit,
        "91nRcYWT", glz::quoted_num<&T::tutorial_dungeon_id>,
        "Ieq49JDy", glz::quoted_num<&T::recover_time_action>,
        "0BPn68DG", glz::quoted_num<&T::recover_time_fight>,
        "YR4HI56k", glz::quoted_num<&T::arena_battle_time_limit>,
        "yFRYDj67", glz::quoted_num<&T::arena_need_mission_id>,
        "WHy3BSm9", glz::quoted_num<&T::max_party_deck_count>,
        "6W4PdoJY", &T::arena_tutorial_npc_info,
        "6GXx4LgZ", glz::quoted_num<&T::verify_flag>,
        "2inP0tCg", glz::quoted_num<&T::unit_mix_great_exp_rate>,
        "zn65EXYF", glz::quoted_num<&T::unit_mix_super_exp_rate>,
        "3xAsgHL8", glz::quoted_num<&T::recover_time_frohun>,
        "zkD98Hfy", glz::quoted_num<&T::recover_time_raid>,
        "S92Hcor3", glz::quoted_num<&T::raid_max_p>,
        "09EbcDmX", glz::quoted_num<&T::raid_bag_count>,
        "3Ep5akHJ", glz::quoted_num<&T::friend_ext_count>,
        "xq0fSrw3", glz::quoted_num<&T::medal_max_num>,
        "MFz8YRS6", &T::compaign_flag,
        "1JFcDr05", glz::quoted_num<&T::max_achieve_point>,
        "SAb3m9wo", glz::quoted_num<&T::zel_per_achieve_point>,
        "KCG5f1AN", glz::quoted_num<&T::karma_per_achieve_point>,
        "4ARtfF7x", glz::quoted_num<&T::max_achieve_point_zel_per_day>,
        "K0sUIn8R", glz::quoted_num<&T::max_achieve_point_karma_per_day>,
        "M1AJuFU6", glz::quoted_num<&T::max_achieve_point_item_per_day>,
        "p3pXbuHA", glz::quoted_num<&T::max_achievement_challenge_accept_count>,
        "kmxPgJu9", glz::quoted_num<&T::max_achievement_record_challenge_accept_count>,
        "uALQnngx", glz::quoted_num<&T::tutorial_gatcha_id>,
        "QylsZTpE", &T::parse_overdrive_param,
        "OjAiNSoh", glz::quoted_num<&T::colosseum_shop_ticket>,
        "924iwrJ9", glz::quoted_num<&T::max_colosseum_ticket>,
        "woxAcRoH", glz::quoted_num<&T::max_cbp>,
        "5csFoG1G", glz::quoted_num<&T::reset_fe_skill_dia_count>,
        "DYMUxgt8", glz::quoted_num<&T::max_blacklist_count>,
        "deYOowYJ", pkg::glaze::array_string<T, &T::init_summoner_arm_id, ','>,
        "QhV0G2zu", glz::quoted_num<&T::max_summoner_sp>,
        "W9bwut7Q", glz::quoted_num<&T::max_summoner_friend_point>,
        "6id2v7eN", glz::quoted_num<&T::max_multi_p>,
        "jFdW1ipx", &T::dbb_crystal_values,
        "7o6lcc66", &T::dbb_fixed_settings_value,
        "hAiXsSPF", &T::action_point_recover_fixed,
        "eRQvzLeF", &T::action_point_threshold
    );
};

template <>
struct glz::meta<NpcPartyInfo> {
    using T = NpcPartyInfo;
    static constexpr auto value = object(
        "P_USER_UNIT_ID", &T::id,
        "P_MEMBER_TYPE", &T::type,
        "P_DISPORDER", &T::disp_order
    );
};

template <>
struct glz::meta<NpcTeamInfo> {
    using T = NpcTeamInfo;
    static constexpr auto value = object(
        "P_USER_ID", &T::user_id,
        "P_LV", &T::lv,
        "P_FRIEND_MESSAGE", &T::friend_message
    );
};

template <>
struct glz::meta<NpcUnitInfo> {
    using T = NpcUnitInfo;
    static constexpr auto value = object(
        "P_UNIT_ID", &T::id,
        "P_USER_UNIT_ID", &T::party_id,
        "P_UNIT_TYPE_ID", &T::type,
        "P_LV", &T::lv,
        "P_HP", &T::hp,
        "P_ATK", &T::atk,
        "P_DEF", &T::deff,
        "P_HEL", &T::hel,
        "P_SKILL_LV", &T::skill_id,
        "P_SKILL_ID", &T::skill_lv,
        "P_EQP_ITEM_ID", &T::equip_item_id
    );
};

template <>
struct glz::meta<NpcMst> {
    using T = NpcMst;
    static constexpr auto value = object(
        "7zyHb5h9", glz::quoted_num<&T::id>,
        "B5JQyV8j", &T::handle_name,
        "JmFn3g9t", glz::quoted_num<&T::arena_rank_id>,
        "g94bDiaS", &T::team,
        "oPsmRC18", &T::parties,
        "bS9s4GCp", &T::units
    );
};

template <>
struct glz::meta<SlotGameInfo> {
    using T = SlotGameInfo;
    static constexpr auto value = object(
        "zS45RFGb", &T::id,
        "I1Cki7Pb", &T::name,
        "h1PSnk5t", &T::reel_pos,
        "b5yeVr61", pkg::glaze::array_string<T, &T::use_medal, '@'>,
        "jsRoN50z", &T::slot_help_url,
        "TX98PnpE", &T::slot_image
    );
};

template <>
struct glz::meta<SlotGamePictureInfo> {
    using T = SlotGamePictureInfo;
    static constexpr auto value = object(
        "sE6tyI9i", &T::id,
        "iQM9dH0F", &T::picture_name
    );
};

template <>
struct glz::meta<SlotGameInfoR> {
    using T = SlotGameInfoR;
    static constexpr auto value = object(
        "C38FmiUn", &T::info,
        "rY6j0Jvs", &T::pictures
    );
};

template <>
struct glz::meta<SlotGameReelInfo> {
    using T = SlotGameReelInfo;
    static constexpr auto value = object(
        "PINm2pM5", &T::id,
        "Z8eJi4pq", &T::reel_data
    );
};

template <>
struct glz::meta<VideoAdInfo> {
    using T = VideoAdInfo;
    static constexpr auto value = object(
        "k3ab6D82", &T::id,
        "Diwl3b56", &T::available_value,
        "Y3de0n2p", &T::region_id,
        "26adZ1iy", glz::bools_as_numbers<&T::video_enabled>,
        "oohpPLCt", &T::next_available_time_left
    );
};

template <>
struct glz::meta<VideoAdRegion> {
    using T = VideoAdRegion;
    static constexpr auto value = object(
        "k3ab6D82", &T::id,
        "j3d6E2ia", &T::country_code
    );
};

template <>
struct glz::meta<VideoAdsSlotGameStandInfo> {
    using T = VideoAdsSlotGameStandInfo;
    static constexpr auto value = object(
        "wRIgGCHh", &T::ads_count,
        "JwBrVzIZ", &T::max_ads_count,
        "BrMgnCaT", &T::current_bouns,
        "E9gMeBW0", &T::max_bouns_count,
        "qqdr4HlW", &T::ads_bonus_flag,
        "er8Ups6U", &T::next_day_timer
    );
};

template <>
struct glz::meta<VideoAdsSlotGameInfo> {
    using T = VideoAdsSlotGameInfo;
    static constexpr auto value = object(
        "C38FmiUn", &T::game_info,
        "iW62Scdg", &T::reel_info,
        "rY6j0Jvs", &T::picture_info,
        "tclBMiv2", &T::game_stand_info
    );
};

template <>
struct glz::meta<BannerInfoMst> {
    using T = BannerInfoMst;
    static constexpr auto value = object(
        "oL71Fz3a", glz::quoted_num<&T::id>,
        "NyYKc1A5", &T::name,
        "aL70hVYQ", pkg::glaze::array_string<T, &T::target_os, ','>,
        "XuJL4pc5", glz::quoted_num<&T::display_order>,
        "jsRoN50z", &T::url,
        "1gDkL6XR", &T::image,
        "t5R47iwj", &T::param,
        "LM34kfVC", &T::page_type,
        "d36D1g8T", &T::read_count
    );
};

template <>
struct glz::meta<ExcludedDungeonMissionMst> {
    using T = ExcludedDungeonMissionMst;
    static constexpr auto value = object(
        "j28VNcUW", &T::id
    );
};

template <>
struct glz::meta<ExtraPassiveSkillMst> {
    using T = ExtraPassiveSkillMst;
    static constexpr auto value = object(
        "cP83zNsv", glz::quoted_num<&T::id>,
        "0nxpBDz2", &T::skill_name,
        "UVN5bGoD", &T::skill_name_s,
        "outas79f", glz::quoted_num<&T::group_id>,
        "yu18xScw", glz::quoted_num<&T::priority>,
        "hjAy9St3", &T::process_id,
        "2Smu5Mtq", &T::process_param,
        "s2gM3deu", &T::term_param,
        "moWQ30GH", glz::quoted_num<&T::target>,
        "qp37xTDh", &T::description,
        "h6UL9A1B", glz::quoted_num<&T::skill_type>,
        "7ofj5xa1", glz::quoted_num<&T::rare>
    );
};

template <>
struct glz::meta<NoticeInfo> {
    using T = NoticeInfo;
    static constexpr auto value = object(
        "xJNom6i0", glz::quoted_num<&T::id>,
        "jsRoN50z", &T::url
    );
};

template <>
struct glz::meta<DailyTaskBonusMst> {
    using T = DailyTaskBonusMst;
    static constexpr auto value = object(
        "k3bD738b", glz::quoted_num<&T::bonus_brave_points>
    );
};

template <>
struct glz::meta<DailyTaskMst> {
    using T = DailyTaskMst;
    static constexpr auto value = object(
        "O36Qv37k", &T::key,
        "hd2Jf3nC", &T::title,
        "M7yKr4c1", &T::desc,
        "Y3DbX5ot", glz::quoted_num<&T::task_count>,
        "T4bV8aI9", glz::quoted_num<&T::task_brave_pts>,
        "bya9a67k", glz::quoted_num<&T::brave_points_total>,
        "22rqpZTo", glz::quoted_num<&T::brave_points>,
        "a3011F8b", &T::area_id,
        "9cKyb15U", &T::times_completed
    );
};

template <>
struct glz::meta<DailyTaskPrizeMst> {
    using T = DailyTaskPrizeMst;
    static constexpr auto value = object(
        "d83aQ39U", glz::quoted_num<&T::id>,
        "T091Rsbe", &T::title,
        "L2VkgH08", &T::desc,
        "30Kw4WBa", glz::quoted_num<&T::present_type>,
        "TdDHf59J", glz::quoted_num<&T::reward_id>,
        "wJsB35iH", glz::quoted_num<&T::reward_count>,
        "37moriMq", &T::reward_param,
        "4NuIwm77", glz::quoted_num<&T::brave_points_cost>,
        "qY49LBjw", &T::time_limit,
        "D2BlS89M", glz::quoted_num<&T::max_claim_count>,
        "jT3oB57e", &T::current_claim_count,
        "J3l5We66", pkg::glaze::bool_as_string<&T::milestone_prize>()
    );
};

template <>
struct glz::meta<SignalKey> {
    using T = SignalKey;
    static constexpr auto value = object(
        "Kn51uR4Y", &T::key
    );
};

template <>
struct glz::meta<ChallengeArenaUserInfo> {
    using T = ChallengeArenaUserInfo;
    static constexpr auto value = object(
        "h7eY3sAK", &T::unk,
        "Nou5bCmm", glz::quoted_num<&T::unkint>,
        "AKP8t3xK", glz::quoted_num<&T::unkint2>,
        "KAZmxkgy", glz::quoted_num<&T::rainbow_coins>,
        "e34YV1Ey", glz::quoted_num<&T::unkint4>,
        "4lH05mQr", glz::quoted_num<&T::unkint5>,
        "xZeGgDQe", glz::quoted_num<&T::league_id>,
        "BcIqcWDM", glz::quoted_num<&T::unkint7>,
        "fBGCdi8I", glz::quoted_num<&T::unkint8>,
        "zf5Ae850", glz::quoted_num<&T::unkint9>,
        "outas79f", &T::unkstr2
    );
};

template <>
struct glz::meta<GuildInfo> {
    using T = GuildInfo;
    static constexpr auto value = object(
        "sD73jd20", glz::quoted_num<&T::id>
    );
};

template <>
struct glz::meta<DailyLoginRewardsUserInfo> {
    using T = DailyLoginRewardsUserInfo;
    static constexpr auto value = object(
        "XIvaD6Jp", glz::quoted_num<&T::id>,
        "35JXN4Ay", glz::quoted_num<&T::user_current_count>,
        "5xStG99s", glz::quoted_num<&T::user_spin_limit_count>,
        "ad6i23pO", glz::quoted_num<&T::current_day>,
        "u8iD6ka7", glz::quoted_num<&T::remaining_days_till_guaranteed_reward>,
        "ZC0msu2L", &T::message,
        "outas79f", glz::quoted_num<&T::next_reward_id>
    );
};

template <>
struct glz::meta<SummonerJournalUserInfo> {
    using T = SummonerJournalUserInfo;
    static constexpr auto value = object(
        "h7eY3sAK", &T::user_id,
        "9hH0neGa", &T::points,
        "da365dB8", &T::journal_flag
    );
};

template <>
struct glz::meta<GuestLogin> {
    using T = GuestLogin;
    static constexpr auto value = object(
        "status", &T::status,
        "token", &T::token,
        "game_user_id", &T::user_id,
        "status_no", glz::quoted_num<&T::status_number>
    );
};

template <>
struct glz::meta<GuestLoginReq> {
    using T = GuestLoginReq;
    static constexpr auto value = object(
        "dn", &T::device_model,
        "dp", &T::device_platform,
        "vid", &T::device_ad_id,
        "v", &T::device_version,
        "altvid", &T::alt_vid,
        "ak", &T::ak,
        "identifiers", &T::identifiers
    );
};

template <>
struct glz::meta<GameDls> {
    using T = GameDls;
    static constexpr auto value = object(
        "game", &T::game_ip,
        "resource", &T::resource_ip,
        "mstv", glz::quoted_num<&T::version>,
        "gumilive", &T::gumilive_ip,
        "bgimage", &T::bg_image,
        "force", &T::force,
        "force_msg", &T::msg
    );
};

template <>
struct glz::meta<Sree> {
    using T = Sree;
    static constexpr auto value = object(
        "SREE", &T::body
    );
};

template <>
struct glz::meta<GumiLiveInfo> {
    using T = GumiLiveInfo;
    static constexpr auto value = object(

    );
};

template <>
struct glz::meta<UserInfoResp> {
    using T = UserInfoResp;
    static constexpr auto value = object(
        "h7eY3sAK", &T::user_id,
        "B5JQyV8j", &T::handle_name,
        "LZ2PfNm4", &T::account_id,
        "4WSu1irc", &T::password,
        "98WfKiyA", &T::friend_id,
        "90LWtVUN", &T::contact_id,
        "9sQM2XcN", glz::quoted_num<&T::tutorial_status>,
        "sv6BEI8X", pkg::glaze::bool_as_string<&T::tutorial_end_flag>(),
        "N4XVE1uA", &T::user_scenario_info,
        "9yVsu21R", &T::user_special_scenario_info,
        "nrg19RGe", glz::quoted_num<&T::model_change_count>,
        "iyJH5k6p", glz::quoted_num<&T::code_expire_date>,
        "y2v7Sd01", glz::quoted_num<&T::friend_invitation_flag>,
        "iN7cYU9i", glz::quoted_num<&T::early_bird_end>,
        "5MPcr0sp", pkg::glaze::bool_as_string<&T::debug_mode>(),
        "8kN1tgYU", &T::encrypt_iv,
        "PA0QwZs1", &T::encrypted_friend_id,
        "7oV00FeR", &T::first_desc,
        "23t3D28i", &T::dlc_url,
        "a37D29iJ", &T::feature_gate,
        "32k0ahkD", &T::unk
    );
};

template <>
struct glz::meta<UserPartyDeckInfo> {
    using T = UserPartyDeckInfo;
    static constexpr auto value = object(
        "U9ABSYEp", &T::deck_type,
        "zsiAn9P1", &T::deck_num,
        "edy7fq3L", &T::user_unit_id,
        "gr48vsdJ", &T::member_type,
        "XuJL4pc5", &T::disp_order
    );
};

template <>
struct glz::meta<UserTeamInfo> {
    using T = UserTeamInfo;
    static constexpr auto value = object(
        "h7eY3sAK", &T::user_id,
        "D9wXQI2V", &T::level,
        "d96tuT2E", &T::exp,
        "YnM14RIP", &T::max_action_point,
        "0P9X1YHs", &T::action_point,
        "9m5FWR8q", &T::max_fight_point,
        "YS2JG9no", &T::fight_point,
        "ouXxIY63", &T::max_unit_count,
        "Px1X7fcd", &T::add_unit_count,
        "QYP4kId9", &T::deck_cost,
        "gEX30r1b", &T::max_equip_slot_count,
        "3u41PhR2", &T::max_friend_count,
        "2rR5s6wn", &T::add_friend_count,
        "J3stQ7jd", &T::friend_point,
        "Najhr8m6", &T::zel,
        "HTVh8a65", &T::karma,
        "03UGMHxF", &T::brave_coin,
        "bM7RLu5K", &T::friend_message,
        "5pjoGBC4", &T::warehouse_count,
        "iI7Wj6pM", &T::add_warehouse_count,
        "s2WnRw9N", &T::want_gift,
        "EfinBo65", &T::present_count,
        "qVBx7g2c", &T::friend_agree_count,
        "1RQT92uE", &T::gift_receive_count,
        "f0IY4nj8", &T::action_rest_timer,
        "jp9s8IyY", &T::fight_rest_timer,
        "92uj7oXB", &T::free_gems,
        "Z0Y4RoD7", &T::active_deck,
        "9r3aLmaB", &T::summon_ticket,
        "s3uU4Lgb", &T::slot_game_flag,
        "KAZmxkgy", &T::rainbow_coin,
        "bya9a67k", &T::brave_points_total,
        "lKuj3Ier", &T::colosseum_ticket,
        "gKNfIZiA", &T::arena_deck_num,
        "TwqMChon", &T::reinforcement_deck,
        "d37CaiX1", &T::paid_gems,
        "Qo9doUsp", &T::mysterybox_count,
        "3a8b9D8i", &T::completed_task_count,
        "7qncTHUJ", &T::inbox_message_count,
        "22rqpZTo", &T::current_brave_points
    );
};

template <>
struct glz::meta<UserUnitInfo> {
    using T = UserUnitInfo;
    static constexpr auto value = object(
        "h7eY3sAK", &T::user_id,
        "edy7fq3L", &T::user_unit_id,
        "pn16CNah", &T::unit_id,
        "nBTx56W9", &T::unit_type_id,
        "D9wXQI2V", &T::unit_lv,
        "d96tuT2E", &T::exp,
        "gQInj3H6", &T::total_exp,
        "e7DK0FQT", &T::base_hp,
        "cuIWp89g", &T::add_hp,
        "TokWs1B3", &T::ext_hp,
        "ISj9u5VL", &T::limit_over_hp,
        "67CApcti", &T::base_atk,
        "RT4CtH5d", &T::add_atk,
        "t4m1RH6Y", &T::ext_atk,
        "D6bKH5eV", &T::limit_over_atk,
        "q08xLEsy", &T::base_def,
        "GcMD0hy6", &T::add_def,
        "e6mY8Z0k", &T::ext_def,
        "3CsiQA0h", &T::limit_over_def,
        "PWXu25cg", &T::base_heal,
        "C1HZr3pb", &T::add_heal,
        "X6jf8DUw", &T::ext_heal,
        "XJs2rPx0", &T::limit_over_heal,
        "iNy0ZU5M", &T::element,
        "oS3kTZ2W", &T::leader_skill_id,
        "nj9Lw7mV", &T::skill_id,
        "3NbeC8AB", &T::skill_lv,
        "iEFZ6H19", &T::extra_skill_id,
        "RQ5GnFE2", &T::extra_skill_lv,
        "Bvkx8s6M", &T::receive_date,
        "5gXxT7LZ", &T::ext_count,
        "0R3qTPK9", &T::equipitem_frame_id,
        "Ge8Yo32T", &T::equipitem_id,
        "RXfC31FA", &T::equipitem_frame_id2,
        "mZA7fH2v", &T::equipitem_id2,
        "dJNpLc81", &T::new_flag,
        "cP83zNsv", &T::extra_passive_skill_id,
        "LjY4DfRg", &T::extra_passive_skill_id2,
        "T4rewHa9", &T::add_extra_passive_skill_id,
        "2pAyFjmZ", &T::unit_img_type,
        "bFQbZh3x", &T::fe_bp,
        "3RgneFpP", &T::fe_used_bp,
        "GIO9DTif", &T::fe_max_usable_bp,
        "Fnxab5CN", &T::fe_skill_info,
        "49sa3sld", &T::omni_level
    );
};

template <>
struct glz::meta<UserInfoReq> {
    using T = UserInfoReq;
    static constexpr auto value = object(
        "B5JQyV8j", &T::handle_name,
        "h7eY3sAK", &T::user_id,
        "90LWtVUN", &T::contact_id,
        "nrg19RGe", glz::quoted_num<&T::model_change_count>,
        "iN7buP0j", &T::device_name,
        "DFY3k6qp", &T::target_os,
        "j2lk52Be", &T::build_platform_id,
        "Ma5GnU0H", &T::device_id,
        "fKSzGDFb", &T::pointer_name,
        "7oV00FeR", &T::first_desc_mst_req,
        "aXf114Oz", &T::notice_mst_list_req,
        "236dItKo", &T::minfo,
        "h7LYasNK", &T::current_language,
        "hceYTcAK", &T::country_code,
        "iN7buP2h", &T::gumi_live_userid,
        "iN7buP1i", &T::gumi_live_token
    );
};

template <>
struct glz::meta<MstUrlList> {
    using T = MstUrlList;
    static constexpr auto value = object(
        "moWQ30GH", &T::id,
        "d2RFtP8T", glz::quoted_num<&T::version>
    );
};

template <>
struct glz::meta<InitializeReq> {
    using T = InitializeReq;
    static constexpr auto value = object(
        "IKqx1Cn9", pkg::glaze::single_array<&T::user_info>(),
        "KeC10fuL", &T::mst_requests
    );
};

template <>
struct glz::meta<InitializeResp> {
    using T = InitializeResp;
    static constexpr auto value = object(
        "Bd29Pqw0", pkg::glaze::single_array<&T::login_campagin>(),
        "bD18x9Ti", &T::login_campaign_reward,
        "YDv9bJ3s", &T::progression,
        "KeC10fuL", &T::mst,
        "Lh1I3dGo", &T::town_facility,
        "d0EkJ4TB", &T::town_facility_lv,
        "1y2JDv79", &T::town_location,
        "9ekQ4tZq", &T::town_location_lv,
        "4NG79sX1", &T::dungeon_keys,
        "6kWq78zx", &T::arena_ranks,
        "Pf97SzVw", &T::gacha_effects,
        "5Y4GJeo3", &T::gachas,
        "VkoZ5t3K", &T::defines,
        "hV5vWu6C", &T::npcs,
        "Pk5F1vhx", &T::banner_info,
        "z5U7utsm", &T::extra_passive_skills,
        "Pj6zDW3m", &T::notice_info,
        "6FrKacq7", pkg::glaze::single_array<&T::signal_key>(),
        "XGmGpmYW", pkg::glaze::single_array<&T::challenge_arena_user_info>(),
        "p283g07d", pkg::glaze::single_array<&T::daily_task_bonuses>(),
        "a739yK18", &T::daily_task_prizes,
        "k23D7d43", &T::daily_tasks,
        "Drudr2w5", &T::daily_login_rewards,
        "IkdSufj5", &T::guild,
        "mebW7mKD", pkg::glaze::single_array<&T::video_ad_slots>(),
        "M3dw18eB", &T::summoner_journal,
        "IKqx1Cn9", pkg::glaze::single_array<&T::user_info>()
    );
};

template <>
struct glz::meta<ItemFavorite> {
    using T = ItemFavorite;
    static constexpr auto value = object(

    );
};

template <>
struct glz::meta<UserLoginCampaignInfo> {
    using T = UserLoginCampaignInfo;
    static constexpr auto value = object(
        "H1Dkq93v", &T::id,
        "ad6i23pO", &T::current_day,
        "1adb38d5", &T::total_days,
        "4tswNoV9", glz::bools_as_numbers<&T::first_for_the_day>
    );
};

template <>
struct glz::meta<MissionBreakInfo> {
    using T = MissionBreakInfo;
    static constexpr auto value = object(

    );
};

template <>
struct glz::meta<NpcMessageOverwriteInfo> {
    using T = NpcMessageOverwriteInfo;
    static constexpr auto value = object(

    );
};

template <>
struct glz::meta<PermitPlace> {
    using T = PermitPlace;
    static constexpr auto value = object(

    );
};

template <>
struct glz::meta<PermitPlaceMl> {
    using T = PermitPlaceMl;
    static constexpr auto value = object(

    );
};

template <>
struct glz::meta<PermitPlaceSp> {
    using T = PermitPlaceSp;
    static constexpr auto value = object(

    );
};

template <>
struct glz::meta<RaidUserInfo> {
    using T = RaidUserInfo;
    static constexpr auto value = object(

    );
};

template <>
struct glz::meta<ReinformenetInfo> {
    using T = ReinformenetInfo;
    static constexpr auto value = object(

    );
};

template <>
struct glz::meta<SummonTicketV2UserInfo> {
    using T = SummonTicketV2UserInfo;
    static constexpr auto value = object(

    );
};

#endif // __has_include(<glaze/glaze.hpp>)
        
