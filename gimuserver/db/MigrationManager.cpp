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
			"level INTEGER(10) NOT NULL DEFAULT 1,"
			"debug_mode INTEGER(1) NOT NULL DEFAULT 0,"
			"exp INTEGER(10) NOT NULL DEFAULT 0,"
			"max_unit_count INTEGER(9) NOT NULL DEFAULT 0,"
			"max_friend_count INTEGER(9) NOT NULL DEFAULT 0,"
			"zel INTEGER(9) NOT NULL DEFAULT 0,"
			"karma INTEGER(9) NOT NULL DEFAULT 0,"
			"brave_coin INTEGER(9) NOT NULL DEFAULT 0,"
			"max_warehouse_count INTEGER(9) NOT NULL DEFAULT 0,"
			"want_gift TEXT NOT NULL DEFAULT '',"
			"friend_points INTEGER(9) NOT NULL DEFAULT 0,"
			"gems INTEGER(4) NOT NULL DEFAULT 0,"
			"active_deck INTEGER(1) NOT NULL DEFAULT 0,"
			"tutorial_status INTEGER(3) NOT NULL DEFAULT 1,"
			"summon_tickets INTEGER(4) NOT NULL DEFAULT 0,"
			"rainbow_coins INTEGER(4) NOT NULL DEFAULT 0,"
			"colosseum_tickets INTEGER(4) NOT NULL DEFAULT 0,"
			"active_arena_deck INTEGER(1) NOT NULL DEFAULT 0,"
			"total_brave_points INTEGER(9) NOT NULL DEFAULT 0,"
			"avail_brave_points INTEGER(9) NOT NULL DEFAULT 0,"
			"energy INTEGER(10) NOT NULL DEFAULT 0,"
			"energy_full_ts INTEGER(10) NOT NULL DEFAULT 0"
			");"
		);
	});

	migrate("08032025_CreateUserUnitsTable", {
		p->execSqlSync(
			"CREATE TABLE IF NOT EXISTS user_units ("
			"user_unit_id INTEGER PRIMARY KEY AUTOINCREMENT,"
			"user_id TEXT NOT NULL,"
			"unit_id INTEGER NOT NULL,"
			"unit_type_id INTEGER NOT NULL,"
			"unit_lvl INTEGER NOT NULL DEFAULT 0,"
			"base_hp INTEGER NOT NULL,"
			"base_atk INTEGER NOT NULL,"
			"base_def INTEGER NOT NULL,"
			"base_rec INTEGER NOT NULL,"
			"ext_hp INTEGER NOT NULL DEFAULT 0,"
			"ext_atk INTEGER NOT NULL DEFAULT 0,"
			"ext_def INTEGER NOT NULL DEFAULT 0,"
			"ext_rec INTEGER NOT NULL DEFAULT 0,"
			"bb_id TEXT NOT NULL DEFAULT '',"
			"bb_lvl INTEGER NOT NULL DEFAULT 0,"
			"sbb_id TEXT NOT NULL DEFAULT '',"
			"sbb_lvl INTEGER NOT NULL DEFAULT 0"
			");"
		);
	});

	migrate("07062026_CreateUserDecksTable", {
		p->execSqlSync(
			"CREATE TABLE IF NOT EXISTS user_decks ("
			"user_id TEXT NOT NULL,"
			"user_unit_id INTEGER NOT NULL,"
			"deck_type INTEGER NOT NULL,"
			"deck_num INTEGER NOT NULL,"
			"member_type INTEGER NOT NULL,"
			"disp_order INTEGER NOT NULL,"
			"PRIMARY KEY (user_id, deck_type, deck_num, disp_order)"
			");"
		);
	});

	// Extra user_units columns used by the quests-branch handlers
	// (UnitMix/UnitEvo/UnitSell/UnitFavorite, GachaAction, FriendGet,
	// CampaignBattleStart).  Consolidates the former
	// 13032025_AddStatsToUserUnitsTable + 09042026_AddSphereSlotsToUserUnits +
	// 14042026_AddFavoriteFlgToUserUnits migrations onto the upstream table
	// shape.  Upstream columns (unit_lvl/base_rec/ext_rec/bb_*) remain the
	// source of truth for upstream handlers; these serve the not-yet-ported
	// quests handlers and are consolidated away as each moves to
	// PacketInterface.
	migrate("02072026_ExtendUserUnitsForUnitOps", {
		p->execSqlSync("ALTER TABLE user_units ADD COLUMN unit_lv INTEGER NOT NULL DEFAULT 1");
		p->execSqlSync("ALTER TABLE user_units ADD COLUMN base_heal INTEGER NOT NULL DEFAULT 0");
		p->execSqlSync("ALTER TABLE user_units ADD COLUMN add_hp INTEGER NOT NULL DEFAULT 0");
		p->execSqlSync("ALTER TABLE user_units ADD COLUMN add_atk INTEGER NOT NULL DEFAULT 0");
		p->execSqlSync("ALTER TABLE user_units ADD COLUMN add_def INTEGER NOT NULL DEFAULT 0");
		p->execSqlSync("ALTER TABLE user_units ADD COLUMN add_heal INTEGER NOT NULL DEFAULT 0");
		p->execSqlSync("ALTER TABLE user_units ADD COLUMN ext_heal INTEGER NOT NULL DEFAULT 0");
		p->execSqlSync("ALTER TABLE user_units ADD COLUMN limit_over_hp INTEGER NOT NULL DEFAULT 0");
		p->execSqlSync("ALTER TABLE user_units ADD COLUMN limit_over_atk INTEGER NOT NULL DEFAULT 0");
		p->execSqlSync("ALTER TABLE user_units ADD COLUMN limit_over_def INTEGER NOT NULL DEFAULT 0");
		p->execSqlSync("ALTER TABLE user_units ADD COLUMN limit_over_heal INTEGER NOT NULL DEFAULT 0");
		p->execSqlSync("ALTER TABLE user_units ADD COLUMN exp INTEGER NOT NULL DEFAULT 1");
		p->execSqlSync("ALTER TABLE user_units ADD COLUMN total_exp INTEGER NOT NULL DEFAULT 1");
		p->execSqlSync("ALTER TABLE user_units ADD COLUMN skill_id INTEGER NOT NULL DEFAULT 0");
		p->execSqlSync("ALTER TABLE user_units ADD COLUMN skill_lv INTEGER NOT NULL DEFAULT 0");
		p->execSqlSync("ALTER TABLE user_units ADD COLUMN extra_skill_id INTEGER NOT NULL DEFAULT 0");
		p->execSqlSync("ALTER TABLE user_units ADD COLUMN extra_skill_lv INTEGER NOT NULL DEFAULT 0");
		p->execSqlSync("ALTER TABLE user_units ADD COLUMN leader_skill_id INTEGER NOT NULL DEFAULT 0");
		p->execSqlSync("ALTER TABLE user_units ADD COLUMN element TEXT NOT NULL DEFAULT 'fire'");
		p->execSqlSync("ALTER TABLE user_units ADD COLUMN fe_bp INTEGER NOT NULL DEFAULT 100");
		p->execSqlSync("ALTER TABLE user_units ADD COLUMN fe_max_usable_bp INTEGER NOT NULL DEFAULT 200");
		// Sphere equipment slots (UserUnitInfo: Ge8Yo32T/0R3qTPK9, mZA7fH2v/RXfC31FA).
		p->execSqlSync("ALTER TABLE user_units ADD COLUMN eqip_item_id INTEGER NOT NULL DEFAULT 0");
		p->execSqlSync("ALTER TABLE user_units ADD COLUMN eqip_item_frame_id INTEGER NOT NULL DEFAULT 0");
		p->execSqlSync("ALTER TABLE user_units ADD COLUMN eqip_item_id2 INTEGER NOT NULL DEFAULT 0");
		p->execSqlSync("ALTER TABLE user_units ADD COLUMN eqip_item_frame_id2 INTEGER NOT NULL DEFAULT 0");
		// Lock/favorite flag (UnitFavoriteRequest: req["3kcmQy7B"][0]["5JbjC3Pp"]).
		p->execSqlSync("ALTER TABLE user_units ADD COLUMN favorite_flg INTEGER NOT NULL DEFAULT 0");
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

	migrate("25042026_CreateUserCampaignTables", {
		p->execSqlSync(
			"CREATE TABLE IF NOT EXISTS user_campaign_missions ("
			"user_id          TEXT    NOT NULL,"
			"mission_id       TEXT    NOT NULL,"
			"state            INTEGER NOT NULL DEFAULT 0,"
			"attain_percent   INTEGER NOT NULL DEFAULT 0,"
			"clear_count      INTEGER NOT NULL DEFAULT 0,"
			"last_cleared_at  INTEGER NOT NULL DEFAULT 0,"
			"reward_claimed   INTEGER NOT NULL DEFAULT 0,"
			"PRIMARY KEY (user_id, mission_id)"
			");"
		);
		p->execSqlSync(
			"CREATE TABLE IF NOT EXISTS user_campaign_decks ("
			"user_id      TEXT    NOT NULL,"
			"deck_num     INTEGER NOT NULL,"
			"member_type  INTEGER NOT NULL,"
			"user_unit_id INTEGER NOT NULL,"
			"disporder    INTEGER NOT NULL DEFAULT 0,"
			"PRIMARY KEY (user_id, deck_num, disporder)"
			");"
		);
		p->execSqlSync(
			"CREATE TABLE IF NOT EXISTS user_campaign_state ("
			"user_id            TEXT PRIMARY KEY,"
			"active_mission_id  TEXT    NOT NULL DEFAULT '',"
			"active_battle_seed INTEGER NOT NULL DEFAULT 0,"
			"saved_state        TEXT    NOT NULL DEFAULT ''"
			");"
		);
	});

	migrate("13052026_CreateUserSummonTicketsV2", {
		p->execSqlSync(
			"CREATE TABLE IF NOT EXISTS user_summon_tickets_v2 ("
			"user_id   TEXT    NOT NULL,"
			"ticket_id INTEGER NOT NULL,"
			"count     INTEGER NOT NULL DEFAULT 0,"
			"PRIMARY KEY (user_id, ticket_id)"
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

