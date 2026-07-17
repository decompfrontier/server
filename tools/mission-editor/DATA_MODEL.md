# Mission Data Model — authoring spec

> Source of truth for the community Mission Editor and for anyone hand-authoring
> `deploy/archive/mission.json`. Every field below is what the server
> (`gimuserver/archive/`) actually reads and how it encodes to the live client.
> Grounded in `archive.hpp` (the structs) and `MissionArchiver.cpp` (the encoder).
>
> Status markers: **[verified]** confirmed against code + a working mission;
> **[UNVERIFIED]** semantics inferred, needs a capture/IDA confirmation before we
> rely on it in the UI.

---

## 1. The big picture

The mobile servers are gone, so mission content has to be **recreated**. The good
news: the runtime is already done. The server serves any mission that has a
**curated archive record**; today only 2 exist (the tutorials). A mission is
plain JSON — no code — so a GUI can create it.

Object graph:

```
Mission (MissionRecord)
├─ header: id, name, zel, karma, exp, energy_cost
└─ stages[]  (waves, played in order)
   └─ Stage (MissionStage): is_boss, first_attack_rate
      └─ enemies[]  (max 6 per stage)
         └─ Enemy (BattleMonster): which unit, where, stats, AI, drops

AI (AiRecord)         ← referenced by each enemy's ai_id, stored separately
Unit pool (UnitRecord) ← backs each enemy's visuals via unit_id
```

Three archive files, all under `deploy/archive/` (path from `config.json`
`archive_root`), loaded **once at server boot** (`MissionArchiver::setup`):

| File | Holds | Count today |
|---|---|---|
| `mission.json` | `MissionRecord[]` — the missions | 2 (tutorials) |
| `ai.json` | `AiRecord[]` — reusable enemy behaviors | 1 ("Attack random") |
| `unit.json` | `UnitRecord[]` — enemy/unit visual+stat pool | 44 |

> **Test loop today:** edit `mission.json` → restart server → `unlockmap` in the
> debug CLI → play in client. A future editor phase adds a hot-reload endpoint so
> "Test" is one click (no restart).

---

## 2. Mission (`MissionRecord`)

| Field | Type | Meaning | Notes |
|---|---|---|---|
| `id` | uint | Mission id. **Must match** the client's `mission_mst` id (what the map tile sends). | e.g. `10`. Duplicate ids in the file → last wins. |
| `name` | string | Human label (server-side/editor only). | Not shown to players; the client uses its own mission text. |
| `zel` | uint | Zel awarded on clear (added to battle drops). | |
| `karma` | uint | Karma awarded on clear. | |
| `exp` | uint | Player EXP awarded on clear (drives level-up). | |
| `energy_cost` | uint | Energy (stamina) consumed at `MissionStart`. | `0` = free. Real Grand Gaia values live in `mission_mst.stamina_cost`. |
| `stages` | Stage[] | Ordered waves. Must have ≥1. | |

The reward fields are authoritative for `MissionEnd` — the editor should default
them from `deploy/system/mission_mst.json` for the matching `id` so recreated
missions pay out correctly.

---

## 3. Stage / wave (`MissionStage`)

| Field | Type | Meaning | Notes |
|---|---|---|---|
| `is_boss` | bool | Marks this wave as the boss battle (client boss framing/music). | Usually the last stage. |
| `first_attack_rate` | uint (0–100) | % chance enemies get a preemptive turn. | `0` = never. |
| `battle_monsters` | Enemy[] | Enemies in this wave. **1–6** (`kMaxMonstersPerStage=6`). | Order = `group_order` on the wire. |

Stages are 1-indexed on the wire (`stageId = index+1`); the editor works in terms
of "Wave 1, Wave 2, …".

---

## 4. Enemy (`BattleMonster`) — the core authoring object

| Field | Type | Meaning | Valid / notes |
|---|---|---|---|
| `id` | uint | Official **monster id** for the client's monster text/MST lookup. | e.g. `30350` "Mossy". Distinct from `unit_id`. |
| `name` | string | Editor/label name. | |
| `unit_id` | uint | **Unit that backs this enemy's sprite + visual/movement data.** | **MUST exist in `unit.json`** or the mission fails to load (`UnitArchiver::lookup`). This bounds the palette — see §7. |
| `position` | `"x:y"` | Battlefield placement (pixels). | **[verified]** from real mission-10 capture. Observed x 96–210, y 248–352; origin top-left, larger x = toward the player (front-right). Canonical early-mission slots by `group_order`: 0→`180:302`, 1→`120:248`, 2→`96:352`. Freeform within the field; the editor canvas should span ~0–320 × ~220–380 with those as snap points. Full 6-slot layout still to confirm from a fuller mission. |
| `hp` | uint | Enemy HP for this mission. | Per-mission override (not from unit stats). |
| `atk` | uint | Enemy ATK. | |
| `def` | uint | Enemy DEF. | |
| `ai_id` | uint | Which `AiRecord` drives its behavior. | Must exist in `ai.json`. |
| `act_min` | uint | Min actions per turn. | Tutorials use 1. |
| `act_max` | uint | Max actions per turn. | Tutorials use 1. |
| `wait` | int | Turn-gauge wait value (turn cadence; wire key `qYCx73y2` on MonsterMst). | Lower ≈ acts sooner. Mission-10 enemies use `5`; tutorials 0–5. Safe default 5. |
| `unit_drop_id` | uint | Unit this enemy can drop (`0` = none). | The dropped unit's base id (e.g. `30030`). |
| `unit_drop_level` | uint | Level of the dropped unit. | |
| `unit_drop_type` | uint | Personality type of the dropped unit. | **[verified]** `F_UNIT_TYPE_MST`: **1–6** (1=Lord/balanced, 2=Anima, 3=Breaker, 4=Guardian, 5=Oracle, 6=special). `1` is the safe default. |
| `unit_drop_chance` | uint (0–100) | Drop % (rolled server-side per battle). | |
| `zel_max_drop` | uint | Max zel this enemy can drop. | Actual is randomized up to this. |
| `zel_drop_count` | uint | Number of zel drop rolls. | |
| `karma_max_drop` | uint | Max karma drop. | |
| `karma_drop_count` | uint | Number of karma rolls. | |
| `treasure_chest_chance` | uint (0–100) | % this enemy yields a treasure chest. | `0` = none. |
| `treasure_drops` | TreasureDrop[] | Weighted table rolled when the chest hits. | |

**Stats vs. visuals split (important for the editor):** an enemy's *combat stats
and behavior* (hp/atk/def/ai/acts/drops/position) come from this record; its
*visuals and movement* (element, attack/hit animation frames, move speed, cursor
offset) come from the `unit.json` record named by `unit_id` (`populatePacket`
merges them). So placing an enemy = "pick a unit for the look, then set its
stats/AI/drops for this fight."

### TreasureDrop

| Field | Type | Meaning |
|---|---|---|
| `target_type` | uint | `1`=zel, `2`=karma, `3`=battle/heart crystal, `4`=item. |
| `weight` | uint | Relative weight in the weighted roll (server picks one entry). |
| `amount` | uint | Zel/karma amount, crystal count, or item count. |
| `target_id` | string | Item id when `target_type=4`; empty otherwise. |

---

## 5. AI (`AiRecord`) — reusable enemy behavior

Stored in `ai.json`, referenced by `enemy.ai_id`. One record = a named behavior =
an ordered list of action rows. Each turn the client walks the rows; the first
whose conditions pass (weighted by `percent`) fires its command.

`AiRecord`: `{ id, name, actions[] }`

`AiAction`:

| Field | Type | Meaning |
|---|---|---|
| `priority` | uint | Row order/index within the AI. |
| `percent` | float | Chance this action fires once its conditions pass. |
| `act_target` | uint | Target pool before `search_term` narrows it. **[UNVERIFIED]** enum. |
| `search_term` | string | How to pick within the pool: `random`, `hp_min`, `hp_max`, … |
| `self_conditions` | SelfCondition[] | Gate on the acting enemy / row (max 5). |
| `party_conditions` | PartyCondition[] | Gate on a player-side target/group (max 4). |
| `action` | Action | The command to run. |

`Action`: `{ type, flag_changes[], unknown_bool, unknown_int_1, unknown_int_2 }`
- `type`: `attack`, `guard`, `wait`, `turn_end`, `skill`, …
- `flag_changes`: AI-flag pairs (max 12; padded with `-1`). **[UNVERIFIED]** grammar.
- the three `unknown_*` are trailing args. **[UNVERIFIED]**

`SelfCondition`: `{ type (skill / hp_pr_under / party_members / …), parameter }`
`PartyCondition`: `{ target_id, target_parameter, type, parameters }`

Minimal usable AI (what `ai.json` ships): priority 1, percent 100, `act_target 2`,
`search_term "random"`, no conditions, action `attack`. The editor's **"🧠 AI behaviors"**
builder (Phase 3) edits `ai.json` directly — create AIs, add prioritized action rows with
type/search/act_target/percent and self/party conditions. The dev-documented vocabulary is offered
as suggestions (free-typed custom values allowed); confirm new values with **Test in game**, since
the wire fields are free-form strings the client parses. Full grammar (all accepted values) still
wants the IDA decode below.

---

## 6. Wire encodings (reference — the editor produces the left side, server emits the right)

From `MissionArchiver.cpp`. Authors never type these; documented so the editor and
reviewers can validate.

- **Unit drop** (per enemy, if rolled): `"{monsterIdx}:0:{unit_drop_id}:{unit_drop_level}:{unit_drop_type}"`.
- **Treasure drop** (weighted pick): `"{monsterIdx}/1/{target_type}:{weight}:{amount}"`, or with `:{target_id}` inserted before amount for items (`type 4`).
- **Mission drop info** (`MissionNumInfo.drop_info`): per stage `"{stageId}|{unitDrops}|{treasureDrops}"`, stages joined by `@`, multiple drops joined by `-`, empty slots are a single space.
- **AI conditions**: `party` terms `"{target_id}:{target_parameter}:{type}:{parameters}@"` ×4 (pad `0:non:non:non@`) + `#` + `self` terms `"{type}:{parameter}@"` ×5 (pad `non:0@`); empty strings encode as `non`.
- **AI action**: `"{type}@{flag},…(×12, pad -1)@{bool01}@{int1}@{int2}"`.

Limits (`MissionArchiver.hpp`): `kMaxMonstersPerStage=6`, `kMaxPartyConditions=4`,
`kMaxSelfConditions=5`, `kMaxActionFlagCount=12`.

---

## 7. Asset pools & how to extend them

**Enemy palette = the 44 units in `unit.json`** (an enemy's `unit_id` must resolve
there). Current pool (id — name — element — rarity): the six starter units, the
Ghost/King/God metal-XP lines per element, the elemental Crystals, the elemental
Imps, Squirty/Mossy/Sparky/Burny starters + King forms, Skeleton, etc. Elements:
1=Fire 2=Water 3=Earth 4=Thunder 5=Light 6=Dark.

To use an enemy **not** in this list, its unit must first be added to `unit.json`
(a `UnitRecord`: visuals `effect_frame`/`damage_frame`, `move_*`, `stats`,
`bb_id`/`sbb_id`). The full 2291-unit roster lives in `deploy/system/unit_mst.json`
— a later editor phase can generate `UnitRecord`s from it so the palette grows.
For v1, authors compose missions from the 44 curated units.

**AI palette = `ai.json`** (1 today). Expand by authoring more `AiRecord`s.

---

## 8. Validation rules the editor must enforce

1. `mission.id` unique across the file; ideally matches a real `mission_mst` id.
2. Each stage has 1–6 enemies; ≥1 stage.
3. Every `enemy.unit_id` exists in `unit.json`; every `enemy.ai_id` exists in `ai.json`.
4. All `*_chance` / `first_attack_rate` in 0–100; `treasure_drops` weights >0.
5. `target_type=4` treasure requires non-empty `target_id`.
6. `position` parses as `x:y` within the canvas bounds.
7. Warn if rewards diverge wildly from `mission_mst` for that id (recreation drift).

---

## 9. Open questions — status

**Reference capture:** a real original-game mission-10 `MissionStart` req/response pair lives at
`C:\Users\Evan\BF\BF-WorkingDir\server\deploy\log_res\jE6Sp0q4_10_04_2026-14_26_53.json` (+ `log_req/`).
Response arrays: `pj41dy9g`=battle groups (5 waves), `75t0sx9z`=monster placement, `89ausgc4`=enemy
skills, `8aiBoHg5`=MonsterMst (`qYCx73y2`=wait), `U0v5IeJo`=monster full stats, `Kz7qfSs5`=drop_info.

**Resolved** (2026-07-17, from the capture + `F_UNIT_TYPE_MST`):
- `position` coordinate space — see §4 (x 96–210, y 248–352; slots 180:302 / 120:248 / 96:352).
- `unit_drop_type` — personality type **1–6** (§4).
- `wait` — turn cadence; mission-10 uses 5 (§4).

**Model scope (verified):** the archive supports **attack-based enemy compositions** — proven by both
tutorials, where *every* enemy including the King Mossy boss uses `ai_id=1` "Attack random" (action
`attack`). That covers early Grand Gaia. Richer behavior (conditional AI, special skills / brave bursts)
is Phase-3 depth (see below).

**Genuinely lost (this is what the editor exists to recreate):** the per-mission battle *composition* —
which monsters, at which positions, with what stats/drops — lived on the dead gumi server. Only
**mission 10** was ever captured (one reference example). Everything else must be authored fresh; there
is no MST to recover it from (the client MSTs hold unit visuals/stats, not mission line-ups).

**Needs an IDA decode (Phase 3 — and this is NEW authoring capability, not recovery):**
- Full **AI grammar** — `act_target` enum, `search_term` vocabulary, condition types, `flag_changes`,
  and how `action` references a **skill** id. The original encodes AI compactly (`89ausgc4` rows like
  `"1:50:1"`); dev's `encodeAIConditions`/`encodeAIAction` produce their own client-accepted format for
  the `attack` preset. Decode the client battle-AI parser via the `tools/ida` readparam workflow — the
  IDB is available (`…/BraveFrontier-APK/lib/arm64-v8a/libgame.so.i64`). Until then **v1 = "pick a preset
  AI."**
- **Enemy special skills / BB** (`Action.type = "skill"`): dev's `Action` carries `unknown_*` args that
  likely hold the skill id; not modelled in the editor yet. Attack-only enemies don't need it.
- Full **6-slot** position layout (mission-10 only shows ≤3/wave).

**Encoder note (not an authoring-model concern):** dev's `encodeUnitDrop` emits
`"{monsterIdx}:0:{id}:{lvl}:{type}"` but the real capture shows `"{monsterIdx}:25:{id}:{lvl}:{type}"`
(field 2 = 25, likely a drop rate/slot). Tutorials drop correctly with dev's `0`, so it's non-blocking.
Contributors never touch this — they set `unit_drop_*`; the server encodes.

---

## 11. Server → client wire mapping (verified against the mission-10 capture)

What `MissionStart` sends and which archive data fills it (`MissionStartResp` in `all.hpp` +
`MissionArchiver::populatePacket`). Confirmed by matching dev's `glz::meta` keys to the real capture's
array element keys.

| Wire key | Array | Archive source | Notes |
|---|---|---|---|
| `6FrKacq7` | signal_key | — | echoed |
| `pj41dy9g` | `BattleGroupMst` | one per **stage** | wave order, boss flag, first-attack |
| `75t0sx9z` | `BattleMonsterGroupMst` | one per **enemy** | `position`, group order, unit-drop hint |
| `U0v5IeJo` | `MonsterMst` | enemy stats + `unit.json` visuals | hp/atk/def/wait/drops + effect/damage frames |
| `89ausgc4` | `AiMst` | `ai.json` rows | **AI action rows, not skills** (keys `q7Nit8JW`=conditions, `VBj9u0ot`=search_term, `Hhgi79M1`=action) |
| `Kz7qfSs5` | `MissionNumInfo` | `encodeMissionDropInfo` | per-stage drop table |
| `9Q1Lq5FS` | `MissionStartInfo` | echo of request | mission id / deck |

The real capture also carried an `8aiBoHg5` (×9) array dev does **not** emit; dev's set is sufficient
(both tutorials play), so it's non-critical — revisit only if a recreated battle misrenders.

---

## 10. Worked example — tutorial mission 1 (annotated)

`deploy/archive/mission.json[0]`: "Basics of Battle", id 1, rewards zel 500 / karma
100 / exp 5, energy 0, **3 stages**:
1. one Mossy (unit 30030, pos 180:302, 300/290/50, AI 1) that drops a Mossy unit (100%).
2. Sparky + Mossy; the second Mossy has a treasure chest (100%) with a 50/50 zel(500)/karma(250) table.
3. boss King Mossy (unit 30031, 3000 HP) — `is_boss: true`.

That single record exercises every major concept: stages, placement, per-enemy
stats, unit drops, zel/karma drops, a weighted treasure table, and a boss flag.
