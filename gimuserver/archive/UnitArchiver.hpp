#pragma once

#include <gimuserver/archive/archive.hpp>
#include <gimuserver/packets/all.hpp>

#include <json/value.h>

#include <cstdint>
#include <stdexcept>
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
	static UnitArchiver& Instance();

	/*!
	* Loads unit archive records from archive_root/unit.json.
	* @param serverObj Server configuration object from the Drogon plugin config.
	*/
	void Setup(const Json::Value& serverObj);

	/*!
	* Looks up a unit archive record.
	*
	* The caller must set record.unit_id before calling. When found, the
	* matching archive record replaces record.
	* @param record Input lookup key and output archive record.
	* @return True if the archive contains the unit, false otherwise.
	*/
	bool Lookup(UnitRecord& record);

	/*!
	* Looks up a unit archive record and converts it to the DB-backed subset of
	* UserUnitInfo.
	*
	* The caller must set unit.unit_id and unit.unit_type_id before calling.
	* Ownership fields such as user_id and user_unit_id are not modified.
	* @param unit Input lookup/type keys and output user unit info.
	* @return True if the archive contains the unit, false otherwise.
	*/
	bool Lookup(UserUnitInfo& unit);

	/*!
	* Gets the stat block for a unit type.
	*
	* Currently only unit_type_id 1 is understood and maps to lord_stats.
	* @throws std::runtime_error when the unit type is not supported.
	*/
	static const UnitRecordStats& UnitTypeStats(const UnitRecord& unitRecord, UnitType unit_type_id)
	{
		switch (unit_type_id)
		{
		case 1:
			return unitRecord.lord_stats;
		default:
			throw std::runtime_error("Unsupported unit type " + std::to_string(unit_type_id));
		}
	}

	UnitArchiver(const UnitArchiver&) = delete;
	UnitArchiver& operator=(const UnitArchiver&) = delete;

private:
	using UnitRecordCache = std::unordered_map<UnitId, UnitRecord>;

	UnitArchiver() = default;

	UnitRecordCache cache_;
};
