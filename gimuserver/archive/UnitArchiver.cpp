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
		cache_.insert_or_assign(unit.id, std::move(unit));
	}

	LOG_INFO << "Loaded " << cache_.size()
		<< " unit archive records from " << archiveRoot << "/unit.json";
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
	// Grab the base stats for the given unit type.
	const auto& stats = unitRecord.stats[0];

	// TODO: Accept a unit level here and compute
	// stats from both the unit type and level.

	unit.unit_id = unitRecord.id;
	unit.unit_lvl = 1;
	unit.unit_type_id = unit_type_id;
	unit.bb_id = unitRecord.bb_id;
	unit.bb_lvl = 1;
	unit.sbb_id = "";
	unit.sbb_lvl = 0;
	unit.base_hp = stats.hp;
	unit.base_atk = stats.atk;
	unit.base_def = stats.def;
	unit.base_rec = stats.rec;
	unit.ext_hp = 0;
	unit.ext_atk = 0;
	unit.ext_def = 0;
	unit.ext_rec = 0;

	return true;
}

