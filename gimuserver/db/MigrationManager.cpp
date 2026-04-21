#include "App.hpp"
#include "MigrationManager.hpp"

using MigrationEntry = std::pair<std::string, std::function<void(drogon::orm::DbClientPtr&)>>;
using MigrationMap = std::vector<MigrationEntry>;

#define migrate(name, func) map.emplace_back(name, [](drogon::orm::DbClientPtr& p) func )

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

	migrate("08032025_CreateUserUnitsTable", {
		p->execSqlSync(
			"CREATE TABLE IF NOT EXISTS user_units ("
			"id INTEGER PRIMARY KEY AUTOINCREMENT,"
			"user_id TEXT NOT NULL,"
			"unit_id TEXT NOT NULL,"
			"UNIQUE(user_id, unit_id)"
			");"
		);
		// Bump AUTOINCREMENT to >= 10000; the client crashes on instance ids < ~10407.
		p->execSqlSync(
			"INSERT INTO user_units (id, user_id, unit_id) VALUES (10000, '__sentinel__', '__sentinel__');"
		);
		p->execSqlSync(
			"DELETE FROM user_units WHERE user_id = '__sentinel__';"
		);
	});

	migrate("13032025_AddStatsToUserUnitsTable", {
		p->execSqlSync("ALTER TABLE user_units ADD COLUMN unit_lv INTEGER DEFAULT 1");
		p->execSqlSync("ALTER TABLE user_units ADD COLUMN base_hp INTEGER DEFAULT 1000");
		p->execSqlSync("ALTER TABLE user_units ADD COLUMN add_hp INTEGER DEFAULT 100");
		p->execSqlSync("ALTER TABLE user_units ADD COLUMN ext_hp INTEGER DEFAULT 100");
		p->execSqlSync("ALTER TABLE user_units ADD COLUMN limit_over_hp INTEGER DEFAULT 200");
		p->execSqlSync("ALTER TABLE user_units ADD COLUMN base_atk INTEGER DEFAULT 1000");
		p->execSqlSync("ALTER TABLE user_units ADD COLUMN add_atk INTEGER DEFAULT 100");
		p->execSqlSync("ALTER TABLE user_units ADD COLUMN ext_atk INTEGER DEFAULT 100");
		p->execSqlSync("ALTER TABLE user_units ADD COLUMN limit_over_atk INTEGER DEFAULT 200");
		p->execSqlSync("ALTER TABLE user_units ADD COLUMN base_def INTEGER DEFAULT 1000");
		p->execSqlSync("ALTER TABLE user_units ADD COLUMN add_def INTEGER DEFAULT 100");
		p->execSqlSync("ALTER TABLE user_units ADD COLUMN ext_def INTEGER DEFAULT 100");
		p->execSqlSync("ALTER TABLE user_units ADD COLUMN limit_over_def INTEGER DEFAULT 200");
		p->execSqlSync("ALTER TABLE user_units ADD COLUMN base_heal INTEGER DEFAULT 1000");
		p->execSqlSync("ALTER TABLE user_units ADD COLUMN add_heal INTEGER DEFAULT 100");
		p->execSqlSync("ALTER TABLE user_units ADD COLUMN ext_heal INTEGER DEFAULT 100");
		p->execSqlSync("ALTER TABLE user_units ADD COLUMN limit_over_heal INTEGER DEFAULT 200");
		p->execSqlSync("ALTER TABLE user_units ADD COLUMN exp INTEGER DEFAULT 1");
		p->execSqlSync("ALTER TABLE user_units ADD COLUMN total_exp INTEGER DEFAULT 1");
		p->execSqlSync("ALTER TABLE user_units ADD COLUMN skill_id INTEGER DEFAULT 0");
		p->execSqlSync("ALTER TABLE user_units ADD COLUMN skill_lv INTEGER DEFAULT 0");
		p->execSqlSync("ALTER TABLE user_units ADD COLUMN extra_skill_id INTEGER DEFAULT 0");
		p->execSqlSync("ALTER TABLE user_units ADD COLUMN extra_skill_lv INTEGER DEFAULT 0");
		p->execSqlSync("ALTER TABLE user_units ADD COLUMN leader_skill_id INTEGER DEFAULT 0");
		p->execSqlSync("ALTER TABLE user_units ADD COLUMN element TEXT DEFAULT 'fire'");
		p->execSqlSync("ALTER TABLE user_units ADD COLUMN fe_bp INTEGER DEFAULT 100");
		p->execSqlSync("ALTER TABLE user_units ADD COLUMN fe_max_usable_bp INTEGER DEFAULT 200");
		p->execSqlSync("ALTER TABLE user_units ADD COLUMN unit_type_id INTEGER DEFAULT 1");
	});

	// Sphere equipment slots (UserUnitInfo: Ge8Yo32T/0R3qTPK9, mZA7fH2v/RXfC31FA).
	migrate("09042026_AddSphereSlotsToUserUnits", {
		p->execSqlSync("ALTER TABLE user_units ADD COLUMN eqip_item_id      INTEGER NOT NULL DEFAULT 0");
		p->execSqlSync("ALTER TABLE user_units ADD COLUMN eqip_item_frame_id INTEGER NOT NULL DEFAULT 0");
		p->execSqlSync("ALTER TABLE user_units ADD COLUMN eqip_item_id2      INTEGER NOT NULL DEFAULT 0");
		p->execSqlSync("ALTER TABLE user_units ADD COLUMN eqip_item_frame_id2 INTEGER NOT NULL DEFAULT 0");
	});

	// Lock/favorite flag (UnitFavoriteRequest: req["3kcmQy7B"][0]["5JbjC3Pp"]).
	migrate("14042026_AddFavoriteFlgToUserUnits", {
		p->execSqlSync("ALTER TABLE user_units ADD COLUMN favorite_flg INTEGER NOT NULL DEFAULT 0");
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

