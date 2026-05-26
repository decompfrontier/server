#include "UnitArchiver.hpp"

#include <gimuserver/utils/JsonFile.hpp>

#include <drogon/drogon.h>

#include <cassert>
#include <exception>
#include <string>
#include <utility>
#include <vector>

UnitArchiver& UnitArchiver::Instance()
{
	static UnitArchiver instance;
	return instance;
}

void UnitArchiver::Setup(const Json::Value& serverObj)
{
	LOG_INFO << "Setting up unit archiver cache. "
		"If you see this after initialization, it is a bug.";

	const auto archiveRoot = serverObj["archive_root"].asString();

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

bool UnitArchiver::Lookup(UnitRecord& record)
{
	// Callers should populate this key since we use it for lookups.
	assert (record.unit_id > 0);

	const auto it = cache_.find(record.unit_id);
	if (it == cache_.end())
	{
		return false;
	}

	record = it->second;
	return true;
}

bool UnitArchiver::Lookup(UserUnitInfo& unit)
{
	// Callers should populate these keys since we use them for lookups.
	assert (unit.unit_id > 0);
	assert (unit.unit_type_id > 0);

	UnitRecord record = {
		.unit_id = unit.unit_id,
	};

	if (!Lookup(record))
	{
		return false;
	}

	// Grab the stats for the unit type.
	const auto& stats = UnitTypeStats(record, unit.unit_type_id);
	unit.base_hp = stats.hp;
	unit.base_atk = stats.atk;
	unit.base_def = stats.def;
	unit.base_rec = stats.rec;

	return true;
}
