# MST Porting Backlog

Tracking remaining MSTs in `C:\Users\Evan\BF\21900\mst-decoded\2-mst-decoded\`
that have NOT been ported to `deploy/system/`.  Each entry shows: source
filename, row count, recommended target name, and the reason it's deferred.
(The copy at `C:\Users\Evan\Desktop\BraveFrontier IDA readParam and Handler
exports and scripts\2-mst-decoded\` is byte-identical — same 147 files.)

See handbook §8.37 for the 23-MST port pass that landed (skill, item,
unit_evo, etc.) and §3.3.1 for the F_UNKNOWN inventory.

**Audit 2026-07-17** (full folder diff, `2-mst-decoded` vs `deploy/system`):
147 source files = 127 distinct MST tables; **33 ported, 94 remaining**.
Every remaining table is accounted for by the sections below.  The audit
found exactly one table missing from this backlog — F_FE_SKILL_MST — now
listed under "Other deferred MSTs".  The F_UNKNOWN section consolidates
the non-minigame unknowns that previously lived only in handbook §3.3.1.

## Newly discovered gap (2026-07-17 audit)

| Source | Rows | Target name | Status |
|---|---:|---|---|
| F_FE_SKILL_MST_Ver618.json | 2899 | `fe_skill_mst.json` | **was missing from this backlog entirely.**  Frontier/FE skill definitions — distinct from F_UNIT_FE_SKILL_MST (already ported as `unit_fe_skill_mst.json`, `kXes8fSi`).  Wrapper key TBD — no `FESkillMstResponse` in mst_responses. |

## Cluster: Frontier Gate (4 files) — needed when FG feature wires up

| Source | Rows | Target name | Status |
|---|---:|---|---|
| F_FROGATE_MST_Ver562.json | 94 | `frontier_gate_mst.json` | wrapper key TBD (no `FrontierGateMstResponse` in mst_responses) |
| F_FROGATE_REWARD_MST_Ver315.json | 1452 | `frontier_gate_reward_mst.json` | wrapper key TBD |
| F_FROGATE_SUPPORT_MST_Ver153.json | 61 | `frontier_gate_support_mst.json` | candidate: `FrontierSupportMstResponse` → `uxpnA5rF` (verify) |
| F_UNKNOWN_MZGk4Tx5 (= F_FRONTIER_GATE_AREA_MST) | 3 | `frontier_gate_area_mst.json` | wrapper key TBD (audit'd as `FrontierGateAreaMstResponse` per §8.36 but not in mst_responses) |

## Cluster: Achievement (3 files) — large data, no current handler

| Source | Rows | Target name | Status |
|---|---:|---|---|
| F_ACHIEVEMENT_DELIVER_RATE_MST_Ver18.json | 53 | `achievement_deliver_rate_mst.json` | wrapper key TBD |
| F_ACHIEVEMENT_SUBJECT_MST_*.json (3 parts) | 5058 | `achievement_subject_mst.json` | wrapper key TBD |
| F_ACHIEVEMENT_TRADE_MST_*.json (4 parts) | 8125 | `achievement_trade_mst.json` | wrapper key TBD |

## Cluster: Grand Mission (10 files) — Grand Gaia world map data

| Source | Rows | Target name |
|---|---:|---|
| F_GRAND_MISSION_END_CND_MST_Ver55 | 46 | `grand_mission_end_cnd_mst.json` |
| F_GRAND_MISSION_EVENT_MST_Ver155 | 965 | `grand_mission_event_mst.json` |
| F_GRAND_MISSION_FLG_MST_Ver102 | 1420 | `grand_mission_flg_mst.json` |
| F_GRAND_MISSION_ICON_MST_Ver53 | 118 | `grand_mission_icon_mst.json` |
| F_GRAND_MISSION_MAP_MST_Ver67 | 28 | `grand_mission_map_mst.json` |
| F_GRAND_MISSION_MST_Ver127 | 26 | `grand_mission_mst.json` |
| F_GRAND_MISSION_REWARD_MST_Ver108 | 371 | `grand_mission_reward_mst.json` |
| F_GRAND_MISSION_ROUTE_MST_Ver92 | 357 | `grand_mission_route_mst.json` |
| F_GRAND_MISSION_SPOT_MST_Ver92 | 325 | `grand_mission_spot_mst.json` |
| F_GRAND_MISSION_TREASURE_MST_Ver43 | 30 | `grand_mission_treasure_mst.json` |

Wrapper keys exist in mst_responses as `Campaign*MstResponse` family
(e.g. `CampaignMapMstResponse → iyCP4N1h`).  Note IDA's "Campaign" =
the in-game "Grand Mission" / Grand Gaia subsystem.

## Cluster: Raid (14 files) — large dataset

| Source | Rows | Target name |
|---|---:|---|
| F_RAID_BATTLE_GROUP_MST_Ver52 | 11121 | `raid_battle_group_mst.json` |
| F_RAID_BOSS_MST_Ver76 | 287 | `raid_boss_mst.json` |
| F_RAID_BOSS_PARTS_MST_Ver78 | 865 | `raid_boss_parts_mst.json` |
| F_RAID_BOSS_ROUTE_MST_Ver45 | 347 | `raid_boss_route_mst.json` |
| F_RAID_MAP_MST_Ver54 | 44 | `raid_map_mst.json` |
| F_RAID_MISSION_BOSS_MST_Ver68 | 264 | `raid_mission_boss_mst.json` |
| F_RAID_MISSION_CLEAR_CND_MST_Ver71 | 235 | `raid_mission_clear_cnd_mst.json` |
| F_RAID_MISSION_MST_Ver396 | 154 | `raid_mission_mst.json` |
| F_RAID_MISSION_POINT_MST_Ver77 | 1378 | `raid_mission_point_mst.json` |
| F_RAID_PLAYSTYLE_MST_Ver6 | 6 | `raid_playstyle_mst.json` |
| F_RAID_POINT_MST_Ver57 | 344 | `raid_point_mst.json` |
| F_RAID_RC_MST_Ver11 | 6 | `raid_rc_mst.json` |
| F_RAID_USER_ROUTE_MST_Ver56 | 4135 | `raid_user_route_mst.json` |
| F_RAID_WORLD_MST_Ver12 | 1 | `raid_world_mst.json` |

Wrapper keys mostly exist in mst_responses as `Raid*MstResponse`
family (e.g. `RaidBossMstResponse → 0qK4FXpC`).

## Cluster: Summoner Unit (SU) (11 files) — separate Summoner system

| Source | Rows | Target name |
|---|---:|---|
| F_SU_ABILITY_LEVEL_MST_Ver29 | 389 | `summoner_ability_level_mst.json` |
| F_SU_ABILITY_MST_Ver25 | 18 | `summoner_ability_mst.json` |
| F_SU_ABILITY_UP_MST_Ver19 | 668 | `summoner_ability_up_mst.json` |
| F_SU_ARM_ELEMENT_MST_Ver16 | 276 | `summoner_arm_element_mst.json` |
| F_SU_ARM_LEVEL_MST_Ver19 | 1440 | `summoner_arm_level_mst.json` |
| F_SU_ARM_MST_Ver16 | 23 | `summoner_arm_mst.json` |
| F_SU_ARM_PASSIVE_MST_Ver15 | 163 | `summoner_arm_passive_mst.json` |
| F_SU_ELEMENT_LEVEL_MST_Ver15 | 600 | `summoner_element_level_mst.json` |
| F_SU_EX_SKILL_MST_Ver15 | 59 | `summoner_ex_skill_mst.json` |
| F_SU_IMG_MST_Ver15 | 828 | `summoner_image_mst.json` |
| F_SU_LEVEL_MST_Ver12 | 500 | `summoner_level_mst.json` |

Wrapper keys exist as `Summoner*MstResponse` family
(e.g. `SummonerAbilityMstResponse → aYcBePEP`).

## Other deferred MSTs

| Source | Rows | Target name | Reason |
|---|---:|---|---|
| F_DBB_BOND_RECIPE_MST_Ver10 | 105 | `dbb_bond_recipe_mst.json` | DBB subsystem inactive |
| F_DBB_MST_Ver136 | 141 | `dbb_mst.json` | DBB subsystem inactive |
| F_COLOSSEUM_CLASS_MST_Ver8 | 4 | `colosseum_class_mst.json` | Colosseum not wired |
| F_COLOSSEUM_EXTRA_RULE_MST_Ver3 | 16 | `colosseum_extra_rule_mst.json` | Colosseum not wired |
| F_COLOSSEUM_FORMATION_MST_Ver2 | 6 | `colosseum_formation_mst.json` | Colosseum not wired |
| F_COLOSSEUM_SUPPORT_MST_Ver2 | 4 | `colosseum_support_mst.json` | Colosseum not wired |
| F_ARENA_EXTRA_RULE_MST_Ver4 | 15 | `arena_extra_rule_mst.json` | Arena wired but feature gated |
| F_GUILD_ART_MST_Ver6 | 19 | `guild_art_mst.json` | Guild subsystem inactive |
| F_GUILD_INFO_MST_Ver6 | 180 | `guild_info_mst.json` | Guild subsystem inactive |
| F_GUILD_POINT_EXCHANGE_MST_Ver233 | 8784 | `guild_point_exchange_mst.json` | Guild subsystem inactive |
| F_GUILD_SKILL_DETAILS_MST_Ver13 | 80 | `guild_skill_details_mst.json` | Guild subsystem inactive |
| F_LAND_MST_Ver147 | 26 | `land_mst.json` | wrapper key TBD (no `LandMstResponse` in mst_responses) |
| F_TRIAL_MISSION_MST_Ver72 | 34 | `trial_mission_mst.json` | wrapper key TBD |
| F_MISSION_NPC_UNIT_MST_Ver212 | 228 | `mission_npc_unit_mst.json` | wrapper key TBD; KDL exists with placeholder hash `1e6jxQzf` |
| F_MISSION_SCRIPT_MST_Ver288 | 228 | `mission_script_mst.json` | wrapper key TBD; KDL exists with placeholder hash `N4XVE1uA` |
| F_FUNCTION_RELEASE_MST_Ver10 | 12 | `function_release_mst.json` | wrapper key TBD |
| F_STAMP_MST_Ver8 | 100 | `stamp_mst.json` | no `StampMstResponse` in mst_responses |
| F_STAMP_SET_MST_Ver10 | 6 | `stamp_set_mst.json` | no `StampSetMstResponse` in mst_responses |
| F_UNIT_EVO_CATALYST_COMMENT_MST_Ver139 | 125 | `unit_evo_catalyst_comment_mst.json` | wrapper key TBD |
| F_UNIT_SELECTOR_GACHA_TICKET_MST_Ver106 | 16 | `unit_selector_gacha_ticket_mst.json` | wrapper key TBD; needed for Burst Hero Selector banner |
| F_SCENARIO_CATEGORY_MST_Ver189 | 119 | — | **no port needed** (2026-07-17): no `*MstResponse` class — client-bundled via CDN .dat.  Schema documented in `assets/mst/scenario.kdl`; per-user viewed state lives in `user_scenarios` (Scenario.cpp). |
| F_SCENARIO_MST_Ver308 | 1348 | — | **no port needed** — same as above.  Columns: mC1JcVM9=id, ctAd91iQ=title, VgU78CYj=category, tw0vkye4=script file (mapN-*.txt), A3TSv2fn=bgm.  NOTE: the mission_script KDL placeholder hash `N4XVE1uA` collides with the real scenario_info wire key (LoginInfoReq / RaidUpScenarioInfo) — replace that placeholder before porting mission_script. |
| F_PURCHASE_AGE_LIMIT_MST_Ver2 | 3 | `purchase_age_limit_mst.json` | low priority |

## F_UNKNOWN files not yet ported (non-minigame)

Identified per handbook §3.3.1 / §8.36 (`tools/ida/audits/mst_unknown/`).
Ported already: `Jba1QEOD` → `gacha_category_mst.json`, `gyY4TCm8` →
`pvp_fixed_setting_mst.json`.  The 8 minigame-cluster unknowns are under
Excluded.

| Group hash | Identified as | Rows | Priority |
|---|---|---:|---|
| `IwODP3o4` | F_ET_MISSION_MST → `et_mission_mst.json` | 460 | HIGH when event-token shops go in (pairs with EventTokenInfo handler) |
| `MZGk4Tx5` | F_FRONTIER_GATE_AREA_MST | 3 | with the Frontier Gate cluster above |
| `aIG5sQwe` | F_CHALLENGE_MIS_CLEAR_MST → `challenge_mis_clear_mst.json` | 23 | with Challenge subsystem |
| `Sf8jpQ0V` | F_SP_CHALLENGE_MIS_CLEAR_MST (provisional) | 8 | with SPChallenge |
| `ni16xAsw` | Summoner Journal milestone vs Vortex Arena rank — ambiguous | 192 | pick when either subsystem lands |
| `Gie01w2u` | tutorial deck preset? (LOW confidence) | 6 | revisit on demand |
| `SBcq4h6J` | disp-order helper? (consumed by ItemEditRequest createBody) | 6 | revisit on demand |
| `pRo1Azab` | unit-type passive bonus? (LOW) | 24 | revisit on demand |
| `y38bDaMv` | F_GUILD_MEMBER_SKILL_MST | 4 | with Guild subsystem |

## Excluded (never to port)

| Source | Rows | Reason |
|---|---:|---|
| F_RESOURCE_BASE_MST_Ver24 | 2744 | Asset manifest, not gameplay data |
| F_RESOURCE_MST_*.json (2 parts) | 24671 | Huge asset manifest, not gameplay data |
| F_EFFECT_GROUP_MST_*.json (2 parts) | 7145 | Battle VFX, not server-side |
| F_EFFECT_MST_Ver583 | 8557 | Battle VFX |
| F_RELOAD_FILE_MST_Ver779 | 562 | Internal reload manifest |
| F_MINIGAME_DEFINE_MST_Ver1 | 31 | Minigame subsystem (deferred per §8.36) |
| F_MINIGAME_MST_Ver1 | 6 | Minigame |
| F_MINIGAME_RESOURCE_MST_Ver2 | 294 | Minigame |
| F_CHRONOLOGY_*.json (4 files) | 0 each | Empty in source |
| 8 F_UNKNOWN files identified as Minigame cluster | varies | See §8.36 |

## Workflow when porting

1. **Look up wrapper key**: open
   `BraveFrontier IDA readParam and Handler exports and scripts/response_mappings.json`,
   find the `<Name>MstResponse` class, copy its `response_key` field.
2. **Add to porter**: append a `PORTS` entry in `tools/port_mst.py`:
   `('F_<NAME>_MST_Ver*.json', '<wrapper_key>', '<target>_mst.json', '<label>', '<response_class>')`
3. **Run**: `python tools/port_mst.py` (idempotent — skips existing).
4. **KDL + ServerCache wiring**: see handbook §8.37 — most ports need
   new KDL written before ServerCache.cpp can load them.
