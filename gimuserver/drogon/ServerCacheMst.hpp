#pragma once

#define auto_cache(key, name) \
struct name##Cache { \
	std::vector<name> data; \
}; \
template <> \
struct glz::meta<name##Cache> { \
using T = name##Cache; \
constexpr static auto value = object(key, &T::data); \
};

auto_cache("bD18x9Ti", LoginCampaignReward)
auto_cache("YDv9bJ3s", UserLevelMst)
auto_cache("KeC10fuL", VersionInfo)
auto_cache("Lh1I3dGo", TownFacilityMst)
auto_cache("d0EkJ4TB", TownFacilityLvMst)
auto_cache("1y2JDv79", TownLocationMst)
auto_cache("9ekQ4tZq", TownLocationLvMst)
auto_cache("hV5vWu6C", NpcMst)
auto_cache("Pk5F1vhx", BannerInfoMst)
auto_cache("z5U7utsm", ExtraPassiveSkillMst)
auto_cache("a739yK18", DailyTaskPrizeMst)
auto_cache("k23D7d43", DailyTaskMst)
auto_cache("4NG79sX1", DungeonKeyMst)
auto_cache("6kWq78zx", ArenaRankMst)
auto_cache("Pf97SzVw", GachaEffectMst)
auto_cache("5Y4GJeo3", GachaMst)
auto_cache("bpD29eiQ", VideoAdRegion)
auto_cache("j129kD6r", VideoAdInfo)
auto_cache("1IR86sAv", GachaInfoMst)
auto_cache("3aDk1xk7", ExcludedDungeonMissionMst)
auto_cache("JYFGe9y6", UnitExpPatternMst)
auto_cache("8f0bCciN", ReceipeMst)
auto_cache("6CTU8m2v", TrophyMst)
auto_cache("1NTG2oVZ", TrophyGroupMst)
auto_cache("Ked15IpH", TrophyGradeMst)
auto_cache("5nBa3CAe", InformationMst)
auto_cache("9x4zZCeN", HelpMst)
auto_cache("At7Gny2V", UrlMst)
auto_cache("mQC4s5ka", ChallengeMst)
auto_cache("h09mEvDR", ChallengeHrMst)
auto_cache("5M8jI4cP", ChallengeMisMst)
auto_cache("zW1i02aG", ChallengeGradeMst)
auto_cache("4C1Wt8sS", ChallengeRewardMst)
auto_cache("dn0NfRy1", ChallengeItemMst)
auto_cache("B8lchAPr", InteractiveBannerInfoMst)
auto_cache("36Sd0Aub", SoundMst)
auto_cache("Bm1WNDQ0", GiftItemMst)
auto_cache("Md9gG3c0", GeneralEventMst)
auto_cache("8UawtzHw", FirstDescMst)
auto_cache("hE1d083b", SummonTicketV2Mst)
auto_cache("da3qD39b", ResummonGachaMst)
auto_cache("5C9LuNrk", HelpSubMst)
auto_cache("P8V71kbw", ChallengeRankRewardMst)
auto_cache("nUmaEC41", ChallengeMvpMst)
