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

	// Remove the UNIQUE(user_id, unit_id) constraint from user_units so the
	// same MST unit can appear multiple times in the inventory (matches real
	// BF behaviour and lets the debug CLI fill the inventory via round-robin).
	// SQLite cannot DROP auto-generated indexes directly; recreate the table
	// without the constraint and copy all existing rows across.
	migrate("24042026_DropUserUnitsUniqueConstraint", {
		p->execSqlSync(
			"CREATE TABLE IF NOT EXISTS user_units_v2 ("
			"id INTEGER PRIMARY KEY AUTOINCREMENT,"
			"user_id TEXT NOT NULL,"
			"unit_id TEXT NOT NULL,"
			"unit_lv INTEGER DEFAULT 1,"
			"base_hp INTEGER DEFAULT 1000, add_hp INTEGER DEFAULT 100,"
			"ext_hp INTEGER DEFAULT 100, limit_over_hp INTEGER DEFAULT 200,"
			"base_atk INTEGER DEFAULT 1000, add_atk INTEGER DEFAULT 100,"
			"ext_atk INTEGER DEFAULT 100, limit_over_atk INTEGER DEFAULT 200,"
			"base_def INTEGER DEFAULT 1000, add_def INTEGER DEFAULT 100,"
			"ext_def INTEGER DEFAULT 100, limit_over_def INTEGER DEFAULT 200,"
			"base_heal INTEGER DEFAULT 1000, add_heal INTEGER DEFAULT 100,"
			"ext_heal INTEGER DEFAULT 100, limit_over_heal INTEGER DEFAULT 200,"
			"exp INTEGER DEFAULT 1, total_exp INTEGER DEFAULT 1,"
			"skill_id INTEGER DEFAULT 0, skill_lv INTEGER DEFAULT 0,"
			"extra_skill_id INTEGER DEFAULT 0, extra_skill_lv INTEGER DEFAULT 0,"
			"leader_skill_id INTEGER DEFAULT 0,"
			"element TEXT DEFAULT 'fire',"
			"fe_bp INTEGER DEFAULT 100, fe_max_usable_bp INTEGER DEFAULT 200,"
			"unit_type_id INTEGER DEFAULT 1,"
			"eqip_item_id INTEGER NOT NULL DEFAULT 0,"
			"eqip_item_frame_id INTEGER NOT NULL DEFAULT 0,"
			"eqip_item_id2 INTEGER NOT NULL DEFAULT 0,"
			"eqip_item_frame_id2 INTEGER NOT NULL DEFAULT 0,"
			"favorite_flg INTEGER NOT NULL DEFAULT 0"
			");"
		);
		p->execSqlSync(
			"INSERT INTO user_units_v2 "
			"(id,user_id,unit_id,unit_lv,base_hp,add_hp,ext_hp,limit_over_hp,"
			"base_atk,add_atk,ext_atk,limit_over_atk,base_def,add_def,ext_def,limit_over_def,"
			"base_heal,add_heal,ext_heal,limit_over_heal,exp,total_exp,"
			"skill_id,skill_lv,extra_skill_id,extra_skill_lv,leader_skill_id,"
			"element,fe_bp,fe_max_usable_bp,unit_type_id,"
			"eqip_item_id,eqip_item_frame_id,eqip_item_id2,eqip_item_frame_id2,favorite_flg)"
			" SELECT id,user_id,unit_id,unit_lv,base_hp,add_hp,ext_hp,limit_over_hp,"
			"base_atk,add_atk,ext_atk,limit_over_atk,base_def,add_def,ext_def,limit_over_def,"
			"base_heal,add_heal,ext_heal,limit_over_heal,exp,total_exp,"
			"skill_id,skill_lv,extra_skill_id,extra_skill_lv,leader_skill_id,"
			"element,fe_bp,fe_max_usable_bp,unit_type_id,"
			"eqip_item_id,eqip_item_frame_id,eqip_item_id2,eqip_item_frame_id2,favorite_flg"
			" FROM user_units;"
		);
		p->execSqlSync("DROP TABLE user_units;");
		p->execSqlSync("ALTER TABLE user_units_v2 RENAME TO user_units;");
		// Ensure AUTOINCREMENT stays >= 10000; the client crashes on ids < ~10407.
		p->execSqlSync(
			"INSERT OR IGNORE INTO sqlite_sequence (name, seq) VALUES ('user_units', 10000);"
		);
		p->execSqlSync(
			"UPDATE sqlite_sequence SET seq = MAX(seq, 10000) WHERE name = 'user_units';"
		);
	});

	// Clamp zel and karma to 99 000 000 on any DB where the CLI set them
	// above the client's safe range.  Values over 99 M cause the client to
	// overflow its internal counter and display / treat the balance as 0,
	// which breaks evo/mix cost checks.
	migrate("24042026_ClampZelKarma", {
		p->execSqlSync(
			"UPDATE userinfo SET zel=MIN(zel,99000000), karma=MIN(karma,99000000)"
			" WHERE id='12345678';"
		);
	});

	// Bump max_unit_count to 4 000 on any DB seeded with the original value of
	// 200.  Level 900 should yield ~4 000 slots; the seed row hard-coded 200.
	migrate("24042026_SetMaxUnitCount", {
		p->execSqlSync(
			"UPDATE userinfo SET max_unit_count=4000 WHERE id='12345678';"
		);
	});

	migrate("25042026_CreateUserTownTables", {
		p->execSqlSync(
			"CREATE TABLE IF NOT EXISTS user_town_facilities ("
			"user_id     TEXT    NOT NULL,"
			"facility_id INTEGER NOT NULL,"
			"lv          INTEGER NOT NULL DEFAULT 1,"
			"karma       INTEGER NOT NULL DEFAULT 0,"
			"PRIMARY KEY (user_id, facility_id)"
			");"
		);
		p->execSqlSync(
			"CREATE TABLE IF NOT EXISTS user_town_locations ("
			"user_id     TEXT    NOT NULL,"
			"location_id INTEGER NOT NULL,"
			"lv          INTEGER NOT NULL DEFAULT 1,"
			"karma       INTEGER NOT NULL DEFAULT 0,"
			"PRIMARY KEY (user_id, location_id)"
			");"
		);
	});

	// Remove event-only town facilities (id >= 1000) that were seeded in error.
	// The packaged client has no sprites for them and crashes on town scene load.
	migrate("25042026_RemoveEventTownFacilities", {
		p->execSqlSync(
			"DELETE FROM user_town_facilities WHERE facility_id >= 1000;"
		);
	});

	// Bump free_gems to 95 000 on any DB where the seed row already existed
	// with the old value of 0.  The original SeedDefaultUserInfo INSERT used
	// free_gems=0; this migration is a targeted one-shot fix.
	migrate("22042026_SetFreeGems", {
		p->execSqlSync(
			"UPDATE userinfo SET free_gems=95000 WHERE id='12345678';"
		);
	});

	migrate("21042026_SeedDefaultUserInfo", {
		p->execSqlSync(
			"INSERT OR IGNORE INTO userinfo ("
			"id, gumi_user_id, device_id, username, level, exp,"
			"max_unit_count, max_friend_count, zel, karma, brave_coin,"
			"max_warehouse_count, free_gems, paid_gems, energy"
			") VALUES ("
			"'12345678','12345678','offline','DecompDev',900,1009680,"
			"4000,100,99000000,99000000,0,"
			"200,95000,99000,398);"
		);
		p->execSqlSync(
			"UPDATE userinfo SET"
			" username='DecompDev',"
			" level=900,"
			" zel=MIN(COALESCE(zel,0),99000000),"
			" karma=MIN(COALESCE(karma,0),99000000),"
			" paid_gems=99000,"
			" free_gems=95000"
			" WHERE id='12345678';"
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

