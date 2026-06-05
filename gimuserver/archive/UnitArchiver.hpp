#pragma once

#include <gimuserver/archive/archive.hpp>
#include <gimuserver/packets/all.hpp>

#include <json/value.h>

#include <cstdint>
#include <optional>
#include <unordered_map>

/*!
* Loads and serves server-owned curated unit archive records.
*
* The archive is intentionally smaller than the official MST data. It contains
* only fields the server understands well enough to use when creating
* player-owned units.
*/
class UnitArchiver final
{
public:
	using UnitId = uint32_t;
	using UnitType = uint32_t;

	/*!
	* Gets the process-wide unit archiver.
	*
	* Setup() must be called during server initialization before request handlers
	* depend on archive data.
	*/
	static UnitArchiver& instance();

	/*!
	* Loads unit archive records from archive_root/unit.json.
	* @param serverObj Server configuration object from the Drogon plugin config.
	*/
	void setup(const Json::Value& serverObj);

	/*!
	* Looks up a unit archive record.
	*
	* @param unit_id Master unit ID from the unit archive.
	* @return Matching immutable archive record, or std::nullopt when not found.
	*/
	std::optional<UnitRecord> lookup(UnitId unit_id) const;

	/*!
	* Converts archive data to a subset of UserUnitInfo.
	*
	* Ownership fields such as user_id and user_unit_id are not modified.
	* @param unitRecord Archive record to read from.
	* @param unit_type_id Unit type to select from the archive stats.
	* @param unit Packet/database object to populate.
	* @return True if the packet was populated, false if the unit type is unsupported.
	*/
	static bool populatePacket(
		const UnitRecord& unitRecord,
		UnitType unit_type_id,
		UserUnitInfo& unit);

	/*!
	* Gets the stat block for a unit type.
	*
	* Currently only unit_type_id 1 is understood and maps to lord_stats.
	* @return Matching stat block, or std::nullopt when the unit type is unsupported.
	*/
	static std::optional<UnitRecordStats> unitTypeStats(
		const UnitRecord& unitRecord,
		UnitType unit_type_id);

	UnitArchiver(const UnitArchiver&) = delete;
	UnitArchiver& operator=(const UnitArchiver&) = delete;

private:
	using UnitRecordCache = std::unordered_map<UnitId, UnitRecord>;

	UnitArchiver() = default;

	UnitRecordCache cache_;
};
