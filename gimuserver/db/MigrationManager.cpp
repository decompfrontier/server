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
	migrate("06022024_CreateUserInfoTable", {
		p->execSqlSync(
			"CREATE TABLE IF NOT EXISTS users("
			"id TEXT PRIMARY KEY,"
			"account_id TEXT,"
			"username TEXT,"
			"admin INTEGER(1)"
			");"
		);
		p->execSqlSync(
			"CREATE TABLE IF NOT EXISTS userinfo("
			"id TEXT PRIMARY KEY,"
			"level INTEGER(3),"
			"exp INTEGER(3),"
			"max_unit_count INTEGER(9),"
			"max_friend_count INTEGER(9),"
			"zel INTEGER(9),"
			"karma INTEGER(9),"
			"brave_coin INTEGER(9),"
			"max_warehouse_count INTEGER(9),"
			"want_gift TEXT,"
			"free_gems INTEGER(4),"
			"paid_gems INTEGER(4),"
			"active_deck INTEGER(1),"
			"summon_tickets INTEGER(4),"
			"rainbow_coins INTEGER(4),"
			"colosseum_tickets INTEGER(4),"
			"active_arena_deck INTEGER(1),"
			"total_brave_points INTEGER(9),"
			"avail_brave_points INTEGER(9),"
			"energy INTEGER(10)"
			");"
		);
	});

	migrate("08032025_CreateUserUnitsTable", {
		p->execSqlSync(
			"CREATE TABLE IF NOT EXISTS user_units ("
			"id INTEGER PRIMARY KEY AUTOINCREMENT," // Add: Auto-incrementing primary key as per PR comment
			"user_id TEXT NOT NULL," // Keep: Links unit to a user
			"unit_id TEXT NOT NULL" // Keep: Stores the unit identifier
			");"
		);
	});
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
		"hash TEXT PRIMARY KEY"
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

	for (const auto& [k, v] : migrations)
	{
		if (std::find(runnedMigratons.begin(), runnedMigratons.end(), k) != runnedMigratons.end())
			continue;

		LOG_INFO << "Execute migration " << k;
		v(ptr);
		ptr->execSqlSync("INSERT INTO migration_status(hash) VALUES ($1);", k);
	}
}

