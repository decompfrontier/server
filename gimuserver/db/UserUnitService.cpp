#include "UserUnitService.hpp"

#include <stdexcept>

drogon::Task<UserUnitService::Result> UserUnitService::addUnit(
	const Database db,
	const std::string_view user_id,
	const UserUnitInfo unit)
{
	if (!db || user_id.empty())
	{
		LOG_ERROR << "Invalid addUnit call: "
			<< "db=" << static_cast<bool>(db)
			<< ", user_id_empty=" << user_id.empty();
		throw std::invalid_argument("Invalid addUnit call");
	}

	co_return co_await db->execSqlCoro(
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
}
