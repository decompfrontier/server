#include "UserInfoService.hpp"

#include <stdexcept>

drogon::Task<UserInfoService::Result> UserInfoService::fetchCurrentGumiUser(
	const Database db,
	std::string& gumi_user_id)
{
	if (!db)
	{
		LOG_ERROR << "Invalid fetchCurrentGumiUser call: "
			<< "db=" << static_cast<bool>(db);
		throw std::invalid_argument("Invalid fetchCurrentGumiUser call");
	}

	auto result = co_await db->execSqlCoro("SELECT id FROM gumi_live_users LIMIT 1;");
	if (!result.empty())
	{
		gumi_user_id = result.front()["id"].as<std::string>();
	}

	co_return result;
}

drogon::Task<UserInfoService::Result> UserInfoService::fetchUserForGumiUser(
	const Database db,
	const std::string_view gumi_user_id,
	std::string& user_id)
{
	if (!db || gumi_user_id.empty())
	{
		LOG_ERROR << "Invalid fetchUserForGumiUser call: "
			<< "db=" << static_cast<bool>(db)
			<< ", gumi_user_id_empty=" << gumi_user_id.empty();
		throw std::invalid_argument("Invalid fetchUserForGumiUser call");
	}

	auto result = co_await db->execSqlCoro(
		"SELECT id FROM userinfo WHERE gumi_user_id = $1 LIMIT 1;",
		gumi_user_id);
	if (!result.empty())
	{
		user_id = result.front()["id"].as<std::string>();
	}

	co_return result;
}

drogon::Task<UserInfoService::Result> UserInfoService::addGumiUser(
	const Database db,
	const std::string_view gumi_user_id)
{
	if (!db || gumi_user_id.empty())
	{
		LOG_ERROR << "Invalid addGumiUser call: "
			<< "db=" << static_cast<bool>(db)
			<< ", gumi_user_id_empty=" << gumi_user_id.empty();
		throw std::invalid_argument("Invalid addGumiUser call");
	}

	co_return co_await db->execSqlCoro(
		"INSERT INTO gumi_live_users(id) VALUES ($1);",
		gumi_user_id);
}

drogon::Task<UserInfoService::Result> UserInfoService::addUser(
	const Database db,
	const std::string_view gumi_user_id,
	const std::string_view user_id,
	const std::string_view username)
{
	if (!db || gumi_user_id.empty() || user_id.empty() || username.empty())
	{
		LOG_ERROR << "Invalid addUser call: "
			<< "db=" << static_cast<bool>(db)
			<< ", gumi_user_id_empty=" << gumi_user_id.empty()
			<< ", user_id_empty=" << user_id.empty()
			<< ", username_empty=" << username.empty();
		throw std::invalid_argument("Invalid addUser call");
	}

	co_return co_await db->execSqlCoro(
		"INSERT INTO userinfo "
		"(id, gumi_user_id, device_id, username, level, max_warehouse_count) "
		"VALUES ($1, $2, $3, $4, $5, $6) "
		"ON CONFLICT(id) DO UPDATE SET "
		"gumi_user_id = excluded.gumi_user_id, "
		"username = excluded.username;",
		user_id,
		gumi_user_id,
		"",
		username,
		1,
		100);
}
