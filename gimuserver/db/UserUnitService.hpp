#pragma once
#include <drogon/orm/DbClient.h>
#include <gimuserver/packets/all.hpp>

#include <cstdint>
#include <string_view>
#include <vector>

/*!
* Stateless database helpers for player-owned units.
*
* This service only persists the UserUnitInfo fields currently backed by the
* user_units table. Fields that are still network-only or not understood yet
* should stay outside this service until the schema is expanded.
*/
class UserUnitService final
{
public:
	using Database = drogon::orm::DbClientPtr;
	using Exception = drogon::orm::DrogonDbException;

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
	* @return True if the unit was added successfully, false otherwise.
	*/
	static drogon::Task<bool> AddUnit(Database db, std::string_view user_id, const UserUnitInfo& unit);

	/*!
	* Gets all units in the user's collection from the local database.
	* Populates only the subset of UserUnitInfo fields currently backed by the
	* user_units table; all other fields keep their default values.
	* @param db Database pointer.
	* @param user_id User ID.
	* @param units Output units.
	* @return True if the units were queried successfully, false otherwise.
	*/
	static drogon::Task<bool> getUnits(Database db, std::string_view user_id, std::vector<UserUnitInfo>& units);
};
