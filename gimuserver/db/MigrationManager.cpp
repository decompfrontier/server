#include "App.hpp"
#include "MigrationManager.hpp"

using MigrationMap = std::unordered_map<std::string, std::function<void(drogon::orm::DbClientPtr& db)>>;

#define migrate(name, func) map.insert_or_assign(name, [](drogon::orm::DbClientPtr& p) func )

/*!
* Register all the available migrations
* @param map Map to register
*/
static void RegisterMigrations(MigrationMap& map)
{
	// DDMMYYYY_MigrationName

	migrate("29082025_CreateDefaultTables", {
		p->execSqlSync(
			"CREATE TABLE gumi_live_users("
			"id TEXT PRIMARY KEY NOT NULL"
			");"
		);
		p->execSqlSync(
			"CREATE TABLE userinfo("
			"id TEXT PRIMARY KEY NOT NULL,"
			"gumi_user_id TEXT NOT NULL,"
			"device_id TEXT NOT NULL,"
			"username TEXT NOT NULL DEFAULT '',"
			"level INTEGER(3) NOT NULL,"
			"debug_mode INTEGER(1) NOT NULL DEFAULT 0,"
			"exp INTEGER(3) NOT NULL DEFAULT 0,"
			"max_unit_count INTEGER(9) NOT NULL DEFAULT 0,"
			"max_friend_count INTEGER(9) NOT NULL DEFAULT 0,"
			"zel INTEGER(9) NOT NULL DEFAULT 0,"
			"karma INTEGER(9) NOT NULL DEFAULT 0,"
			"brave_coin INTEGER(9) NOT NULL DEFAULT 0,"
			"max_warehouse_count INTEGER(9) NOT NULL DEFAULT 0,"
			"want_gift TEXT NOT NULL DEFAULT '',"
			"free_gems INTEGER(4) NOT NULL DEFAULT 0,"
			"paid_gems INTEGER(4) NOT NULL DEFAULT 0,"
			"active_deck INTEGER(1) NOT NULL DEFAULT 0,"
			"summon_tickets INTEGER(4) NOT NULL DEFAULT 0,"
			"rainbow_coins INTEGER(4) NOT NULL DEFAULT 0,"
			"colosseum_tickets INTEGER(4) NOT NULL DEFAULT 0,"
			"active_arena_deck INTEGER(1) NOT NULL DEFAULT 0,"
			"total_brave_points INTEGER(9) NOT NULL DEFAULT 0,"
			"avail_brave_points INTEGER(9) NOT NULL DEFAULT 0,"
			"energy INTEGER(10) NOT NULL DEFAULT 0"
			");"
		);
	});

#if 0
	migrate("08032025_CreateUserUnitsTable", {
		p->execSqlSync(
			"CREATE TABLE IF NOT EXISTS user_units ("
			"id INTEGER PRIMARY KEY AUTOINCREMENT," // Add: Auto-incrementing primary key as per PR comment
			"user_id TEXT NOT NULL," // Keep: Links unit to a user
			"unit_id TEXT NOT NULL" // Keep: Stores the unit identifier
			");"
		);
	});
#endif
}

/*!
* Gets all the runned migration (and optionally create the table)
* @param p Database pointer
* @param hash Output where to store all the hashes
*/
static void GetMigrationStatus(drogon::orm::DbClientPtr& p, std::vector<std::string>& hash)
{
	p->execSqlSync(
		"CREATE TABLE IF NOT EXISTS migration_status("
		"hash TEXT PRIMARY KEY NOT NULL"
		");"
	);
	auto res = p->execSqlSync(
		"SELECT hash FROM migration_status;"
	);
	if (res.size() > 0)
	{
		for (const auto& x : res)
		{
			const auto& str = x[0].as<std::string>();
			LOG_DEBUG << "Migration found: " << str;
			hash.emplace_back(str);
		}
	}
}

void MigrationManager::RunMigrations(drogon::orm::DbClientPtr ptr)
{
	MigrationMap migrations;
	RegisterMigrations(migrations);

	std::vector<std::string> runnedMigratons;
	GetMigrationStatus(ptr, runnedMigratons);

	try
	{
		for (const auto& [k, v] : migrations)
		{
			if (std::find(runnedMigratons.begin(), runnedMigratons.end(), k) != runnedMigratons.end())
				continue;

			LOG_INFO << "Execute migration " << k;
			v(ptr);
			ptr->execSqlSync("INSERT INTO migration_status(hash) VALUES ($1);", k);
		}
	}
	catch (drogon::orm::DrogonDbException ex)
	{
		LOG_ERROR << "Cannot execute migration: " << ex.base().what();
		drogon::app().quit();
	}

}

