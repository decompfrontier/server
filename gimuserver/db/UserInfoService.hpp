#pragma once

#include <gimuserver/db/PacketInterface.hpp>

#include <cstdint>
#include <string>
#include <string_view>

/*!
* Stateless database helpers for core player state.
*/
class UserInfoService final
{
public:
	using Database = drogon::orm::DbClientPtr;
	using Exception = drogon::orm::DrogonDbException;
	using Result = drogon::orm::Result;

	UserInfoService() = delete;

	/*!
	* Fetches the first known Gumi Live user ID from the local database.
	* @param db Database pointer.
	* @param gumi_user_id Output Gumi Live user ID. Left unchanged if no row exists.
	*/
	static drogon::Task<Result> fetchCurrentGumiUser(
		const Database db,
		std::string& gumi_user_id);

	/*!
	* Fetches the first game user ID owned by a Gumi Live user.
	* @param db Database pointer.
	* @param gumi_user_id Gumi Live user ID to look up.
	* @param user_id Output game user ID. Left unchanged if no row exists.
	*/
	static drogon::Task<Result> fetchUserForGumiUser(
		const Database db,
		const std::string_view gumi_user_id,
		std::string& user_id);

	/*!
	* Adds a Gumi Live user row.
	* @param db Database pointer.
	* @param gumi_user_id Gumi Live user ID to persist.
	*/
	static drogon::Task<Result> addGumiUser(
		const Database db,
		const std::string_view gumi_user_id);

	/*!
	* Adds a new game user row with default starting account values.
	* @param db Database pointer.
	* @param gumi_user_id Gumi Live user ID that owns this user.
	* @param user_id Game user ID.
	* @param username Initial in-game handle name.
	*/
	static drogon::Task<Result> addUser(
		const Database db,
		const std::string_view gumi_user_id,
		const std::string_view user_id,
		const std::string_view username);

};
