#include "UnitArchiver.hpp"

#include <gimuserver/utils/JsonFile.hpp>

#include <drogon/drogon.h>

#include <exception>
#include <string>
#include <utility>
#include <vector>

UnitArchiver& UnitArchiver::instance()
{
	static UnitArchiver instance;
	return instance;
}

void UnitArchiver::setup(const Json::Value& serverObj)
{
	LOG_INFO << "Setting up unit archiver cache. "
		"If you see this after initialization, it is a bug.";

	const auto archiveRoot = serverObj["archive_root"].asString();
	if (archiveRoot.empty())
	{
		LOG_ERROR << "Unable to set up unit archiver cache: archive_root is empty";
		return;
	}

	std::vector<UnitRecord> units;
	try
	{
		units = LoadJson<std::vector<UnitRecord>>(archiveRoot, "unit.json");
	}
	catch (const std::exception& ex)
	{
		LOG_ERROR << "Unable to set up unit archiver cache: " << ex.what();
		return;
	}

	cache_.clear();
	cache_.reserve(units.size());
	for (auto& unit : units)
	{
		cache_.insert_or_assign(unit.unit_id, std::move(unit));
	}
}

std::optional<UnitRecord> UnitArchiver::lookup(UnitId unit_id) const
{
	if (unit_id == 0)
	{
		LOG_ERROR << "Invalid unit archive lookup: unit_id is 0";
		return std::nullopt;
	}

	const auto it = cache_.find(unit_id);
	if (it == cache_.end())
	{
		LOG_ERROR << "Unable to find unit archive record " << unit_id;
		return std::nullopt;
	}

	return it->second;
}

bool UnitArchiver::populatePacket(
	const UnitRecord& unitRecord,
	UnitType unit_type_id,
	UserUnitInfo& unit)
{
	// Grab the stats for the given unit type.
	const auto stats = unitTypeStats(unitRecord, unit_type_id);
	if (!stats)
	{
		return false;
	}

	unit.unit_id = unitRecord.unit_id;
	unit.unit_type_id = unit_type_id;
	unit.base_hp = stats->hp;
	unit.base_atk = stats->atk;
	unit.base_def = stats->def;
	unit.base_rec = stats->rec;
	return true;
}

std::optional<UnitRecordStats> UnitArchiver::unitTypeStats(
	const UnitRecord& unitRecord,
	UnitType unit_type_id)
{
	switch (unit_type_id)
	{
	case 1:
		return unitRecord.lord_stats;
	default:
		LOG_ERROR << "Unsupported unit type " << unit_type_id
			<< " for unit archive record " << unitRecord.unit_id;
		return std::nullopt;
	}
}
