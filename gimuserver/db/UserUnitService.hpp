#pragma once

#include <drogon/orm/DbClient.h>
#include <gimuserver/packets/all.hpp>

#include <string_view>

/*!
* Stateless database helpers for player-owned units.
*/
class UserUnitService final
{
public:
	using Database = drogon::orm::DbClientPtr;
	using Result = drogon::orm::Result;

	/*!
	* This service is used only through static methods.
	*/
	UserUnitService() = delete;

	/*!
	* Adds a unit to the user's collection.
	* Persists only the subset of UserUnitInfo fields currently backed by the
	* user_units table.
	* @param db Database pointer.
	* @param user_id User ID.
	* @param unit Unit to add.
	*/
	static drogon::Task<Result> addUnit(
		const Database db,
		const std::string_view user_id,
		const UserUnitInfo unit);
};
