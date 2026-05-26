#include "UserUnitService.hpp"

#include <string>

drogon::Task<bool> UserUnitService::AddUnit(Database db, std::string_view user_id, const UserUnitInfo& unit)
{
	if (!db)
	{
		co_return false;
	}

	try
	{
		const auto result = co_await db->execSqlCoro(
			"INSERT INTO user_units ("
			"user_id, "
			"unit_id, "
			"unit_type_id, "
			"base_hp, "
			"base_atk, "
			"base_def, "
			"base_rec, "
			"ext_hp, "
			"ext_atk, "
			"ext_def, "
			"ext_rec"
			") VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10, $11);",
			user_id,
			unit.unit_id,
			unit.unit_type_id,
			unit.base_hp,
			unit.base_atk,
			unit.base_def,
			unit.base_rec,
			unit.ext_hp,
			unit.ext_atk,
			unit.ext_def,
			unit.ext_rec);

		co_return result.affectedRows() == 1;
	}
	catch (const Exception&)
	{
		co_return false;
	}
}

drogon::Task<bool> UserUnitService::getUnits(
	Database db,
	std::string_view user_id,
	std::vector<UserUnitInfo>& units)
{
	if (!db)
	{
		co_return false;
	}

	try
	{
		const auto result = co_await db->execSqlCoro(
			"SELECT "
			"user_unit_id, "
			"user_id, "
			"unit_id, "
			"unit_type_id, "
			"base_hp, "
			"base_atk, "
			"base_def, "
			"base_rec, "
			"ext_hp, "
			"ext_atk, "
			"ext_def, "
			"ext_rec "
			"FROM user_units "
			"WHERE user_id = $1 "
			"ORDER BY user_unit_id ASC;",
			user_id);

		units.clear();
		units.reserve(result.size());

		for (const auto& row : result)
		{
			// Keep this mapping in lockstep with the selected DB-backed fields.
			UserUnitInfo unit = {};
			unit.user_unit_id = row["user_unit_id"].as<uint32_t>();
			unit.user_id = row["user_id"].as<std::string>();
			unit.unit_id = row["unit_id"].as<uint32_t>();
			unit.unit_type_id = row["unit_type_id"].as<uint32_t>();
			unit.base_hp = row["base_hp"].as<uint32_t>();
			unit.base_atk = row["base_atk"].as<uint32_t>();
			unit.base_def = row["base_def"].as<uint32_t>();
			unit.base_rec = row["base_rec"].as<uint32_t>();
			unit.ext_hp = row["ext_hp"].as<uint32_t>();
			unit.ext_atk = row["ext_atk"].as<uint32_t>();
			unit.ext_def = row["ext_def"].as<uint32_t>();
			unit.ext_rec = row["ext_rec"].as<uint32_t>();
			units.emplace_back(std::move(unit));
		}

		co_return true;
	}
	catch (const Exception&)
	{
		co_return false;
	}
}
