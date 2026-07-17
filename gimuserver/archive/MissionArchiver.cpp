#include "MissionArchiver.hpp"

#include "UnitArchiver.hpp"

#include <gimuserver/utils/JsonFile.hpp>
#include <gimuserver/utils/Random.hpp>

#include <drogon/drogon.h>

#include <algorithm>
#include <cstddef>
#include <exception>
#include <format>
#include <set>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

std::string MissionArchiver::encodeAIConditions(const AiAction& action)
{
	// For string values, the client expects "non" to mean empty,
	// so we default to that when encoding empty strings.
	const auto nonIfEmpty = [](const std::string& value) -> std::string_view {
		return value.empty() ? std::string_view("non") : std::string_view(value);
	};

	std::ostringstream stream;

	const auto partyCnt = std::min(action.party_conditions.size(), kMaxPartyConditions);
	for (size_t i = 0; i < partyCnt; ++i)
	{
		const auto& condition = action.party_conditions[i];
		stream << std::format(
			"{}:{}:{}:{}@",
			condition.target_id,
			nonIfEmpty(condition.target_parameter),
			nonIfEmpty(condition.type),
			nonIfEmpty(condition.parameters));
	}
	for (size_t i = partyCnt; i < kMaxPartyConditions; ++i)
	{
		stream << "0:non:non:non@";
	}

	stream << '#';

	const auto selfCnt = std::min(action.self_conditions.size(), kMaxSelfConditions);
	for (size_t i = 0; i < selfCnt; ++i)
	{
		const auto& condition = action.self_conditions[i];
		stream << std::format(
			"{}:{}@",
			nonIfEmpty(condition.type),
			condition.parameter);
	}
	for (size_t i = selfCnt; i < kMaxSelfConditions; ++i)
	{
		stream << "non:0@";
	}

	return stream.str();
}

std::string MissionArchiver::encodeAIAction(const Action& action)
{
	std::ostringstream stream;
	stream << action.type << '@';
	const auto flagCnt = std::min(action.flag_changes.size(), kMaxActionFlagCount);
	for (size_t i = 0; i < flagCnt; ++i)
	{
		stream << action.flag_changes[i] << ',';
	}
	for (size_t i = flagCnt; i < kMaxActionFlagCount; ++i)
	{
		stream << "-1,";
	}
	stream << std::format(
		"@{}@{}@{}",
		action.unknown_bool ? 1 : 0,
		action.unknown_int_1,
		action.unknown_int_2);
	return stream.str();
}

std::string MissionArchiver::encodeUnitDrop(size_t monsterIdx, const BattleMonster& monster)
{
	if (monster.unit_drop_id == 0 || RandomUInt(1, 100) > monster.unit_drop_chance)
	{
		return "";
	}

	return std::format(
		"{}:0:{}:{}:{}",
		monsterIdx,
		monster.unit_drop_id,
		monster.unit_drop_level,
		monster.unit_drop_type);
}

std::string MissionArchiver::encodeTreasureDrop(size_t monsterIdx, const BattleMonster& monster)
{
	if (monster.treasure_drops.empty()
		|| monster.treasure_chest_chance == 0
		|| RandomUInt(1, 100) > monster.treasure_chest_chance)
	{
		return "";
	}

	uint32_t total = 0;
	std::vector<const TreasureDrop*> drops;
	drops.reserve(monster.treasure_drops.size());
	for (const auto& drop : monster.treasure_drops)
	{
		if (drop.weight == 0 || drop.amount == 0 || !validTreasureType(drop.target_type))
		{
			continue;
		}

		if (drop.target_type == 4 && drop.target_id.empty())
		{
			continue;
		}

		total += drop.weight;
		drops.push_back(&drop);
	}

	if (drops.empty())
	{
		return "";
	}

	// Go through each possible drop and return the selected one.
	auto roll = RandomUInt(1, total);
	for (const auto drop : drops)
	{
		if (roll > drop->weight)
		{
			roll -= drop->weight;
			continue;
		}

		if (drop->target_type == 4)
		{
			return std::format(
				"{}/1/{}:{}:{}:{}",
				monsterIdx,
				drop->target_type,
				drop->weight,
				drop->target_id,
				drop->amount);
		}

		return std::format(
			"{}/1/{}:{}:{}",
			monsterIdx,
			drop->target_type,
			drop->weight,
			drop->amount);
	}

	return "";
}

std::string MissionArchiver::encodeMissionDropInfo(const MissionRecord& record)
{
	std::ostringstream stream;
	for (size_t stageIdx = 0; stageIdx < record.stages.size(); ++stageIdx)
	{
		const auto& stage = record.stages[stageIdx];
		const auto stageId = static_cast<uint32_t>(stageIdx + 1);
		if (stageIdx != 0)
		{
			stream << '@';
		}

		stream << stageId << '|';

		std::string unitDrops;
		std::string treasureDrops;
		for (size_t monsterIdx = 0; monsterIdx < stage.battle_monsters.size(); ++monsterIdx)
		{
			const auto unitDrop = encodeUnitDrop(monsterIdx, stage.battle_monsters[monsterIdx]);
			if (!unitDrop.empty() && !unitDrops.empty())
			{
				unitDrops += '-';
			}
			unitDrops += unitDrop;

			const auto treasureDrop = encodeTreasureDrop(monsterIdx, stage.battle_monsters[monsterIdx]);
			if (!treasureDrop.empty() && !treasureDrops.empty())
			{
				treasureDrops += '-';
			}
			treasureDrops += treasureDrop;
		}

		stream << (unitDrops.empty() ? " " : unitDrops) << '|';
		stream << (treasureDrops.empty() ? " " : treasureDrops);
	}

	return stream.str();
}

MissionArchiver& MissionArchiver::instance()
{
	static MissionArchiver instance;
	return instance;
}

void MissionArchiver::setup(const Json::Value& serverObj)
{
	LOG_INFO << "Setting up mission archiver cache. "
		"If you see this after initialization, it is a bug.";

	const auto archiveRoot = serverObj["archive_root"].asString();
	if (archiveRoot.empty())
	{
		LOG_ERROR << "Unable to set up mission archiver cache: archive_root is empty";
		return;
	}

	archiveRoot_ = archiveRoot;
	const auto result = loadFromRoot(archiveRoot);
	if (result.ok)
	{
		LOG_INFO << "Loaded " << result.missions
			<< " mission archive records and " << result.ais
			<< " AI archive records from " << archiveRoot;
	}
}

MissionArchiver::ReloadResult MissionArchiver::reload()
{
	if (archiveRoot_.empty())
	{
		return { false, 0, 0, "archive root not set (server not initialized)" };
	}

	const auto result = loadFromRoot(archiveRoot_);
	if (result.ok)
	{
		LOG_INFO << "Reloaded " << result.missions << " mission and "
			<< result.ais << " AI archive records from " << archiveRoot_;
	}
	return result;
}

MissionArchiver::ReloadResult MissionArchiver::loadFromRoot(const std::string& archiveRoot)
{
	std::vector<MissionRecord> missions;
	std::vector<AiRecord> ais;
	try
	{
		missions = LoadJson<std::vector<MissionRecord>>(archiveRoot, "mission.json");
		ais = LoadJson<std::vector<AiRecord>>(archiveRoot, "ai.json");
	}
	catch (const std::exception& ex)
	{
		LOG_ERROR << "Unable to load mission archive: " << ex.what();
		return { false, 0, 0, ex.what() };
	}

	AiRecordCache nextAiCache;
	nextAiCache.reserve(ais.size());
	for (auto& ai : ais)
	{
		nextAiCache.insert_or_assign(ai.id, std::move(ai));
	}

	MissionRecordCache nextMissionCache;
	nextMissionCache.reserve(missions.size());
	for (auto& mission : missions)
	{
		nextMissionCache.insert_or_assign(mission.id, std::move(mission));
	}

	const size_t missionCount = nextMissionCache.size();
	const size_t aiCount = nextAiCache.size();

	// Swap under lock so a concurrent lookup()/populatePacket() never observes a
	// half-rehashed map (the caches are otherwise touched on the request path).
	{
		std::lock_guard<std::mutex> lock(cacheMutex_);
		aiCache_ = std::move(nextAiCache);
		missionCache_ = std::move(nextMissionCache);
	}

	return { true, missionCount, aiCount, "" };
}

std::optional<MissionRecord> MissionArchiver::lookup(MissionId mission_id) const
{
	std::lock_guard<std::mutex> lock(cacheMutex_);
	const auto it = missionCache_.find(mission_id);
	if (it == missionCache_.end())
	{
		LOG_ERROR << "Unable to find mission archive record " << mission_id;
		return std::nullopt;
	}

	return it->second;  // copy returned while locked — safe against reload()
}

bool MissionArchiver::populatePacket(const MissionRecord& record, std::vector<AiMst>& msts)
{
	// Gather unique AI ids referenced by the mission record.
	std::set<AiId> ids;
	for (const auto& stage : record.stages)
	{
		for (const auto& monster : stage.battle_monsters)
		{
			ids.insert(monster.ai_id);
		}
	}
	if (ids.empty())
	{
		LOG_ERROR << "Unable to populate AI MST rows: mission " << record.id
			<< " does not reference any AI";
		return false;
	}

	msts.clear();
	// Most tutorial AI records have one action, but records can expand to
	// multiple MST rows when they contain multiple actions.
	msts.reserve(ids.size());
	// Hold the cache lock while reading aiCache_ so a hot-reload can't
	// invalidate the referenced AI records mid-iteration.
	std::lock_guard<std::mutex> lock(instance().cacheMutex_);
	for (const auto id : ids)
	{
		const auto it = instance().aiCache_.find(id);
		if (it == instance().aiCache_.end())
		{
			LOG_ERROR << "Unable to populate AI MST rows: missing AI archive record " << id;
			return false;
		}

		const auto& aiRecord = it->second;
		if (aiRecord.actions.empty())
		{
			LOG_ERROR << "Unable to populate AI MST rows: AI archive record "
				<< id << " has no actions";
			return false;
		}

		for (const auto& action : aiRecord.actions)
		{
			msts.push_back({
				.ai_id = aiRecord.id,
				.priority = action.priority,
				.name = aiRecord.name,
				.conditions = encodeAIConditions(action),
				.act_target = action.act_target,
				.search_term = action.search_term,
				.action = encodeAIAction(action.action),
				.percent = action.percent
			});
		}
	}

	return true;
}

bool MissionArchiver::populatePacket(const MissionRecord& record, std::vector<MonsterMst>& msts)
{
	msts.clear();
	msts.reserve(record.stages.size() * kMaxMonstersPerStage);
	for (size_t stageIdx = 0; stageIdx < record.stages.size(); ++stageIdx)
	{
		const auto& stage = record.stages[stageIdx];
		for (size_t monsterIdx = 0; monsterIdx < stage.battle_monsters.size(); ++monsterIdx)
		{
			const auto& monster = stage.battle_monsters[monsterIdx];
			// Visual and movement fields come from the curated unit archive.
			const auto unit = UnitArchiver::instance().lookup(monster.unit_id);
			if (!unit)
			{
				LOG_ERROR << "Unable to populate monster MST rows: missing unit archive record "
					<< monster.unit_id << " for mission " << record.id;
				return false;
			}

			msts.push_back({
				.monster_id = monster.id,
				.base_name = std::to_string(monster.id),
				.hp = monster.hp,
				.atk = monster.atk,
				.def = monster.def,
				.element = unit->element,
				.effect_frame = unit->effect_frame,
				.damage_frame = unit->damage_frame,
				.drop_check_count = unit->drop_check_count,
				.unit_id = unit->id,
				.move_speed = unit->move_speed,
				.attack_move_type = unit->attack_move_type,
				.back_move_type = unit->back_move_type,
				.ai_id = monster.ai_id,
				.after_image = unit->after_image,
				.max_act_count = monster.act_max,
				.min_act_count = monster.act_min,
				.skill_move_type = unit->skill_move_type,
				.max_zel_drop = monster.zel_max_drop,
				.zel_drop_count = monster.zel_drop_count,
				.max_karma_drop = monster.karma_max_drop,
				.karma_drop_count = monster.karma_drop_count,
				.wait = monster.wait,
				.cursor_disp_pos = unit->cursor_disp_pos
			});
		}
	}

	if (msts.empty())
	{
		LOG_ERROR << "Unable to populate monster MST rows: mission " << record.id
			<< " has no monsters";
		return false;
	}

	return true;
}

bool MissionArchiver::populatePacket(
	const MissionRecord& record,
	std::vector<BattleMonsterGroupMst>& msts)
{
	msts.clear();
	msts.reserve(record.stages.size() * kMaxMonstersPerStage);
	for (size_t stageIdx = 0; stageIdx < record.stages.size(); ++stageIdx)
	{
		const auto& stage = record.stages[stageIdx];
		const auto stageId = static_cast<uint32_t>(stageIdx + 1);
		for (size_t monsterIdx = 0; monsterIdx < stage.battle_monsters.size(); ++monsterIdx)
		{
			const auto& monster = stage.battle_monsters[monsterIdx];

			msts.push_back({
				.battle_monster_group_id = stageId,
				.monster_id = monster.id,
				.group_order = static_cast<uint32_t>(monsterIdx),
				.position = monster.position,
				// The client does not use this field to resolve unit drops; it uses the
				// encoded drop info in MissionNumInfo instead. This value only tells the
				// client which unit assets may be needed, so keep the other fields as zero.
				.unit_drop = (monster.unit_drop_id == 0 || monster.unit_drop_chance == 0)
					? "0:0:0:0"
					: std::format("0:{}:0:0", monster.unit_drop_id)
			});
		}
	}

	if (msts.empty())
	{
		LOG_ERROR << "Unable to populate battle monster group MST rows: mission "
			<< record.id << " has no monsters";
		return false;
	}

	return true;
}

bool MissionArchiver::populatePacket(const MissionRecord& record, std::vector<BattleGroupMst>& msts)
{
	if (record.stages.empty())
	{
		LOG_ERROR << "Unable to populate battle group MST rows: mission "
			<< record.id << " has no stages";
		return false;
	}

	msts.clear();
	msts.reserve(record.stages.size());

	for (size_t stageIdx = 0; stageIdx < record.stages.size(); ++stageIdx)
	{
		const auto& stage = record.stages[stageIdx];
		const auto stageId = static_cast<uint32_t>(stageIdx + 1);

		msts.push_back({
			.battle_group_id = stageId,
			.battle_monster_group_id = stageId,
			.mission_id = record.id,
			.battle_order = stageId,
			.first_attack_rate = stage.first_attack_rate,
			.boss_flag = stage.is_boss
		});
	}

	return true;
}

bool MissionArchiver::populatePacket(const MissionRecord& record, MissionNumInfo& mst)
{
	mst.serial_id = record.id;
	mst.drop_info = encodeMissionDropInfo(record);
	return true;
}
