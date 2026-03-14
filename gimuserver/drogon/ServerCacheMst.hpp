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
