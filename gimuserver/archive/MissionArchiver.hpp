#pragma once

#include <gimuserver/archive/archive.hpp>
#include <gimuserver/packets/all.hpp>

#include <drogon/drogon.h>

#include <json/value.h>

#include <cstddef>
#include <cstdint>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

/*!
* Loads and serves server-owned curated mission archive records.
*
* The archive stores logical mission data only. Client battle group ids and
* encoded AI/drop wire strings are generated while formatting a mission-start
* response.
*/
class MissionArchiver final
{
public:
	using MissionId = uint32_t;

	/*!
	* Gets the process-wide mission archiver.
	*
	* Setup() must be called during server initialization before request handlers
	* depend on archive data.
	*/
	static MissionArchiver& instance();

	/*!
	* Loads mission and AI archive records from archive_root.
	* @param serverObj Server configuration object from the Drogon plugin config.
	*/
	void setup(const Json::Value& serverObj);

	/// Outcome of a hot-reload, for the authoring-tool endpoint.
	struct ReloadResult {
		bool ok = false;
		size_t missions = 0;
		size_t ais = 0;
		std::string error;
	};

	/*!
	* Re-reads mission.json + ai.json from the archive root captured at setup()
	* and atomically swaps the caches. Lets the mission-authoring tool load newly
	* saved missions without a full server restart. Thread-safe against lookup().
	*
	* @return Counts on success, or ok=false with an error message.
	*/
	ReloadResult reload();

	/*!
	* Looks up a mission archive record.
	*
	* @param mission_id Mission id from mission.json.
	* @return Matching archive record, or std::nullopt when not found.
	*/
	std::optional<MissionRecord> lookup(MissionId mission_id) const;

	/*!
	* Populates all AI MST rows referenced by the mission record.
	*
	* @param record Archive record to read from.
	* @param msts Packet rows to populate.
	* @return True if the packet rows were populated, false if referenced AI
	* archive data is missing or invalid.
	*/
	static bool populatePacket(const MissionRecord& record, std::vector<AiMst>& msts);

	/*!
	* Populates all monster MST rows referenced by the mission record.
	*
	* @param record Archive record to read from.
	* @param msts Packet rows to populate.
	* @return True if the packet rows were populated, false if referenced unit
	* archive data is missing or the mission has no monsters.
	*/
	static bool populatePacket(const MissionRecord& record, std::vector<MonsterMst>& msts);

	/*!
	* Populates all battle monster group MST rows referenced by the mission
	* record.
	*
	* @param record Archive record to read from.
	* @param msts Packet rows to populate.
	* @return True if the packet rows were populated, false if the mission has no
	* monsters.
	*/
	static bool populatePacket(const MissionRecord& record, std::vector<BattleMonsterGroupMst>& msts);

	/*!
	* Populates all battle group MST rows referenced by the mission record.
	*
	* @param record Archive record to read from.
	* @param msts Packet rows to populate.
	* @return True if the packet rows were populated, false if the mission has no
	* stages.
	*/
	static bool populatePacket(const MissionRecord& record, std::vector<BattleGroupMst>& msts);

	/*!
	* Populates mission number data for the mission-start response.
	*
	* @param record Archive record to read from.
	* @param mst Packet object to populate.
	* @return True if the packet was populated.
	*/
	static bool populatePacket(const MissionRecord& record, MissionNumInfo& mst);

	MissionArchiver(const MissionArchiver&) = delete;
	MissionArchiver& operator=(const MissionArchiver&) = delete;

private:
	using AiId = uint32_t;
	using MissionRecordCache = std::unordered_map<MissionId, MissionRecord>;
	using AiRecordCache = std::unordered_map<AiId, AiRecord>;

	static constexpr size_t kMaxPartyConditions = 4;
	static constexpr size_t kMaxSelfConditions = 5;
	static constexpr size_t kMaxActionFlagCount = 12;
	static constexpr size_t kMaxMonstersPerStage = 6;

	MissionArchiver() = default;

	/*!
	* Encodes the condition string for one AI action.
	*
	* @param action Archive action to read from.
	* @return Client-formatted condition string.
	*/
	static std::string encodeAIConditions(const AiAction& action);

	/*!
	* Encodes the command string for one AI action.
	*
	* @param action Archive action command to read from.
	* @return Client-formatted action string.
	*/
	static std::string encodeAIAction(const Action& action);

	/*!
	* Selects and encodes the unit drop for one mission monster.
	*
	* Unit drops are resolved server-side. The client only receives the final
	* result: whether a unit dropped, and which unit it was.
	*
	* @param monsterIdx Zero-based monster index inside the stage.
	* @param monster Archive monster to read from.
	* @return Client-formatted unit drop string, or an empty string when no unit
	* drops.
	*/
	static std::string encodeUnitDrop(size_t monsterIdx, const BattleMonster& monster);

	/*!
	* Selects and encodes the treasure drop for one mission monster.
	*
	* Treasure rolls are resolved server-side. The client only receives the final
	* result: whether a chest dropped, and which reward that chest contains.
	*
	* @param monsterIdx Zero-based monster index inside the stage.
	* @param monster Archive monster to read from.
	* @return Client-formatted treasure drop string, or an empty string when no
	* treasure drops.
	*/
	static std::string encodeTreasureDrop(size_t monsterIdx, const BattleMonster& monster);

	/*!
	* Encodes all selected mission drops for MissionNumInfo.
	*
	* @param record Archive record to read from.
	* @return Client-formatted mission drop info string.
	*/
	static std::string encodeMissionDropInfo(const MissionRecord& record);

	/*!
	* Loads mission.json + ai.json from a root and swaps the caches under lock.
	* Shared by setup() and reload().
	*
	* @param archiveRoot Directory holding the archive JSON files.
	* @return Counts on success, or ok=false with an error message.
	*/
	ReloadResult loadFromRoot(const std::string& archiveRoot);

	/*!
	* Checks whether a treasure drop target type is supported.
	*
	* @param targetType Treasure target type from mission.json.
	* @return True if the treasure target type can be encoded.
	*/
	static bool validTreasureType(uint32_t targetType)
	{
		switch (targetType)
		{
		case 1:
		case 2:
		case 3:
		case 4:
			return true;
		default:
			LOG_ERROR << "Unsupported treasure drop target type " << targetType;
			return false;
		}
	}

	// Guards the caches so a hot-reload can't race a concurrent lookup/populate.
	mutable std::mutex cacheMutex_;
	// Archive directory captured at setup() so reload() can re-read the files.
	std::string archiveRoot_;
	MissionRecordCache missionCache_;
	AiRecordCache aiCache_;
};
