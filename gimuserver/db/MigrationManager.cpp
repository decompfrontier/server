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

	// Campaign subsystem tables.  user_campaign_missions tracks per-mission
	// progress (state: 0=locked, 1=available, 2=cleared).  user_campaign_decks
	// holds the campaign-specific party composition.  user_campaign_state holds
	// transient state for the active mission/battle.
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

	// Seed a placeholder mission catalog for the dev account.  These IDs are
	// synthetic — real campaign MST loading is a future task.  All three start
	// available so the Campaign UI has clickable tiles.
	migrate("25042026_SeedCampaignMissions", {
		p->execSqlSync(
			"INSERT OR IGNORE INTO user_campaign_missions"
			" (user_id, mission_id, state, attain_percent)"
			" VALUES ('12345678','10001',1,0),"
			"        ('12345678','10002',1,0),"
			"        ('12345678','10003',1,0);"
		);
		p->execSqlSync(
			"INSERT OR IGNORE INTO user_campaign_state (user_id) VALUES ('12345678');"
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

	// Clamp gem values to 4 200 — values in the tens-of-thousands overflow the
	// client's gem counter and get silently reset to 0.  4 200 sits comfortably
	// under the client's safe display limit.
	migrate("27042026_SetGemsTo4200", {
		p->execSqlSync(
			"UPDATE userinfo SET paid_gems=4200, free_gems=4200"
			" WHERE id='12345678';"
		);
	});

	// Create the user_party_decks table that DeckEdit writes to and UserInfo
	// reads from.  No seed data — UserInfo falls back to a generated default
	// deck the first time a user logs in, and DeckEdit persists edits from then
	// on.
	migrate("27042026_CreateUserPartyDecks", {
		p->execSqlSync(
			"CREATE TABLE IF NOT EXISTS user_party_decks ("
			"user_id      TEXT    NOT NULL,"
			"deck_type    INTEGER NOT NULL DEFAULT 1,"
			"deck_num     INTEGER NOT NULL,"
			"user_unit_id INTEGER NOT NULL DEFAULT 0,"
			"member_type  INTEGER NOT NULL DEFAULT 0,"
			"disp_order   INTEGER NOT NULL DEFAULT 0,"
			"PRIMARY KEY (user_id, deck_type, deck_num, disp_order)"
			");"
		);
	});

	// Broaden campaign mission availability: seed missions 1-200 as available
	// under the correct UUID so the client's campaign MST can match its node IDs
	// against the server list regardless of exact numbering scheme.
	// INSERT OR IGNORE means already-cleared missions keep their existing state.
	migrate("28042026_BroadenCampaignMissions", {
		p->execSqlSync(
			"INSERT OR IGNORE INTO user_campaign_missions"
			" (user_id, mission_id, state, attain_percent)"
			" VALUES"
			" ('0839899613932562','21',1,0),('0839899613932562','22',1,0),"
			" ('0839899613932562','23',1,0),('0839899613932562','24',1,0),"
			" ('0839899613932562','25',1,0),('0839899613932562','26',1,0),"
			" ('0839899613932562','27',1,0),('0839899613932562','28',1,0),"
			" ('0839899613932562','29',1,0),('0839899613932562','30',1,0),"
			" ('0839899613932562','31',1,0),('0839899613932562','32',1,0),"
			" ('0839899613932562','33',1,0),('0839899613932562','34',1,0),"
			" ('0839899613932562','35',1,0),('0839899613932562','36',1,0),"
			" ('0839899613932562','37',1,0),('0839899613932562','38',1,0),"
			" ('0839899613932562','39',1,0),('0839899613932562','40',1,0),"
			" ('0839899613932562','41',1,0),('0839899613932562','42',1,0),"
			" ('0839899613932562','43',1,0),('0839899613932562','44',1,0),"
			" ('0839899613932562','45',1,0),('0839899613932562','46',1,0),"
			" ('0839899613932562','47',1,0),('0839899613932562','48',1,0),"
			" ('0839899613932562','49',1,0),('0839899613932562','50',1,0);"
		);
		p->execSqlSync(
			"INSERT OR IGNORE INTO user_campaign_missions"
			" (user_id, mission_id, state, attain_percent)"
			" VALUES"
			" ('0839899613932562','51',1,0),('0839899613932562','52',1,0),"
			" ('0839899613932562','53',1,0),('0839899613932562','54',1,0),"
			" ('0839899613932562','55',1,0),('0839899613932562','56',1,0),"
			" ('0839899613932562','57',1,0),('0839899613932562','58',1,0),"
			" ('0839899613932562','59',1,0),('0839899613932562','60',1,0),"
			" ('0839899613932562','61',1,0),('0839899613932562','62',1,0),"
			" ('0839899613932562','63',1,0),('0839899613932562','64',1,0),"
			" ('0839899613932562','65',1,0),('0839899613932562','66',1,0),"
			" ('0839899613932562','67',1,0),('0839899613932562','68',1,0),"
			" ('0839899613932562','69',1,0),('0839899613932562','70',1,0),"
			" ('0839899613932562','71',1,0),('0839899613932562','72',1,0),"
			" ('0839899613932562','73',1,0),('0839899613932562','74',1,0),"
			" ('0839899613932562','75',1,0),('0839899613932562','76',1,0),"
			" ('0839899613932562','77',1,0),('0839899613932562','78',1,0),"
			" ('0839899613932562','79',1,0),('0839899613932562','80',1,0);"
		);
		p->execSqlSync(
			"INSERT OR IGNORE INTO user_campaign_missions"
			" (user_id, mission_id, state, attain_percent)"
			" VALUES"
			" ('0839899613932562','81',1,0),('0839899613932562','82',1,0),"
			" ('0839899613932562','83',1,0),('0839899613932562','84',1,0),"
			" ('0839899613932562','85',1,0),('0839899613932562','86',1,0),"
			" ('0839899613932562','87',1,0),('0839899613932562','88',1,0),"
			" ('0839899613932562','89',1,0),('0839899613932562','90',1,0),"
			" ('0839899613932562','91',1,0),('0839899613932562','92',1,0),"
			" ('0839899613932562','93',1,0),('0839899613932562','94',1,0),"
			" ('0839899613932562','95',1,0),('0839899613932562','96',1,0),"
			" ('0839899613932562','97',1,0),('0839899613932562','98',1,0),"
			" ('0839899613932562','99',1,0),('0839899613932562','100',1,0),"
			" ('0839899613932562','101',1,0),('0839899613932562','102',1,0),"
			" ('0839899613932562','103',1,0),('0839899613932562','104',1,0),"
			" ('0839899613932562','105',1,0),('0839899613932562','106',1,0),"
			" ('0839899613932562','107',1,0),('0839899613932562','108',1,0),"
			" ('0839899613932562','109',1,0),('0839899613932562','110',1,0);"
		);
		p->execSqlSync(
			"INSERT OR IGNORE INTO user_campaign_missions"
			" (user_id, mission_id, state, attain_percent)"
			" VALUES"
			" ('0839899613932562','111',1,0),('0839899613932562','112',1,0),"
			" ('0839899613932562','113',1,0),('0839899613932562','114',1,0),"
			" ('0839899613932562','115',1,0),('0839899613932562','116',1,0),"
			" ('0839899613932562','117',1,0),('0839899613932562','118',1,0),"
			" ('0839899613932562','119',1,0),('0839899613932562','120',1,0),"
			" ('0839899613932562','121',1,0),('0839899613932562','122',1,0),"
			" ('0839899613932562','123',1,0),('0839899613932562','124',1,0),"
			" ('0839899613932562','125',1,0),('0839899613932562','126',1,0),"
			" ('0839899613932562','127',1,0),('0839899613932562','128',1,0),"
			" ('0839899613932562','129',1,0),('0839899613932562','130',1,0),"
			" ('0839899613932562','131',1,0),('0839899613932562','132',1,0),"
			" ('0839899613932562','133',1,0),('0839899613932562','134',1,0),"
			" ('0839899613932562','135',1,0),('0839899613932562','136',1,0),"
			" ('0839899613932562','137',1,0),('0839899613932562','138',1,0),"
			" ('0839899613932562','139',1,0),('0839899613932562','140',1,0),"
			" ('0839899613932562','141',1,0),('0839899613932562','142',1,0),"
			" ('0839899613932562','143',1,0),('0839899613932562','144',1,0),"
			" ('0839899613932562','145',1,0),('0839899613932562','146',1,0),"
			" ('0839899613932562','147',1,0),('0839899613932562','148',1,0),"
			" ('0839899613932562','149',1,0),('0839899613932562','150',1,0),"
			" ('0839899613932562','151',1,0),('0839899613932562','152',1,0),"
			" ('0839899613932562','153',1,0),('0839899613932562','154',1,0),"
			" ('0839899613932562','155',1,0),('0839899613932562','156',1,0),"
			" ('0839899613932562','157',1,0),('0839899613932562','158',1,0),"
			" ('0839899613932562','159',1,0),('0839899613932562','160',1,0),"
			" ('0839899613932562','161',1,0),('0839899613932562','162',1,0),"
			" ('0839899613932562','163',1,0),('0839899613932562','164',1,0),"
			" ('0839899613932562','165',1,0),('0839899613932562','166',1,0),"
			" ('0839899613932562','167',1,0),('0839899613932562','168',1,0),"
			" ('0839899613932562','169',1,0),('0839899613932562','170',1,0),"
			" ('0839899613932562','171',1,0),('0839899613932562','172',1,0),"
			" ('0839899613932562','173',1,0),('0839899613932562','174',1,0),"
			" ('0839899613932562','175',1,0),('0839899613932562','176',1,0),"
			" ('0839899613932562','177',1,0),('0839899613932562','178',1,0),"
			" ('0839899613932562','179',1,0),('0839899613932562','180',1,0),"
			" ('0839899613932562','181',1,0),('0839899613932562','182',1,0),"
			" ('0839899613932562','183',1,0),('0839899613932562','184',1,0),"
			" ('0839899613932562','185',1,0),('0839899613932562','186',1,0),"
			" ('0839899613932562','187',1,0),('0839899613932562','188',1,0),"
			" ('0839899613932562','189',1,0),('0839899613932562','190',1,0),"
			" ('0839899613932562','191',1,0),('0839899613932562','192',1,0),"
			" ('0839899613932562','193',1,0),('0839899613932562','194',1,0),"
			" ('0839899613932562','195',1,0),('0839899613932562','196',1,0),"
			" ('0839899613932562','197',1,0),('0839899613932562','198',1,0),"
			" ('0839899613932562','199',1,0),('0839899613932562','200',1,0);"
		);
	});

	// Rename the userinfo primary key from the legacy account-ID '12345678' to
	// the canonical UUID '0839899613932562' so every table uses a single user
	// identity.  All subsequent handler code queries userinfo by the UUID.
	migrate("28042026_MigrateUserInfoId", {
		p->execSqlSync(
			"UPDATE userinfo"
			" SET id='0839899613932562', gumi_user_id='0839899613932562'"
			" WHERE id='12345678';"
		);
	});

	// Set skill_lv=10 / extra_skill_lv=10 for all existing user_units rows that
	// have a non-zero skill_id / extra_skill_id but still have the old default
	// of 0.  Without skill_lv > 0 the battle engine treats the BB as unlearned
	// and the gauge never activates.  Mirrors the old-tree PopulateUnitMstTable
	// migration that did the same UPDATE with CASE WHEN … THEN 10 END.
	migrate("29042026_FixSkillLevels", {
		p->execSqlSync(
			"UPDATE user_units"
			" SET skill_lv = 10"
			" WHERE skill_id > 0 AND skill_lv = 0;"
		);
		p->execSqlSync(
			"UPDATE user_units"
			" SET extra_skill_lv = 10"
			" WHERE extra_skill_id > 0 AND extra_skill_lv = 0;"
		);
	});

	// The initial unit seeding ran with a mismatched parameter order in
	// InsertUnitFromMst, storing the element string in extra_skill_lv and a
	// stringified integer in element.  UserInfo.cpp calls
	// row["extra_skill_lv"].as<int32_t>() which throws std::invalid_argument on
	// a TEXT value, crashing the handler before any response is sent.
	// Fix: delete all corrupted units and their stale party-deck references so
	// that GimuServer::SeedDefaultUnits re-seeds them with the corrected INSERT
	// on the very next server start (it is idempotent: skips when count > 0).
	migrate("03052026_FixCorruptedUnitData", {
		p->execSqlSync(
			"DELETE FROM user_party_decks WHERE user_id='0839899613932562';"
		);
		p->execSqlSync(
			"DELETE FROM user_units WHERE user_id='0839899613932562';"
		);
	});

	// Fix the campaign mission seed: previous migration inserted rows under
	// user_id='12345678' (the userinfo PK) but the CampaignStart / CampaignMissionGet
	// handlers query by user_id='0839899613932562' (the login/units UUID).
	// Delete the wrong rows and re-insert under the correct ID, then broaden the
	// mission catalog so the Campaign UI has more tiles to render.
	migrate("27042026_FixCampaignMissionSeeding", {
		// Remove wrongly-keyed rows.
		p->execSqlSync(
			"DELETE FROM user_campaign_missions WHERE user_id='12345678';"
		);
		p->execSqlSync(
			"DELETE FROM user_campaign_state WHERE user_id='12345678';"
		);
		// Re-seed under the correct UUID with a broader initial catalog.
		// Mission IDs 1-20 cover the first chapter; additional IDs can be added
		// once the exact MST values are known from capture logs.
		p->execSqlSync(
			"INSERT OR IGNORE INTO user_campaign_missions"
			" (user_id, mission_id, state, attain_percent)"
			" VALUES"
			" ('0839899613932562','1',1,0),"
			" ('0839899613932562','2',1,0),"
			" ('0839899613932562','3',1,0),"
			" ('0839899613932562','4',1,0),"
			" ('0839899613932562','5',1,0),"
			" ('0839899613932562','6',1,0),"
			" ('0839899613932562','7',1,0),"
			" ('0839899613932562','8',1,0),"
			" ('0839899613932562','9',1,0),"
			" ('0839899613932562','10',1,0),"
			" ('0839899613932562','11',1,0),"
			" ('0839899613932562','12',1,0),"
			" ('0839899613932562','13',1,0),"
			" ('0839899613932562','14',1,0),"
			" ('0839899613932562','15',1,0),"
			" ('0839899613932562','16',1,0),"
			" ('0839899613932562','17',1,0),"
			" ('0839899613932562','18',1,0),"
			" ('0839899613932562','19',1,0),"
			" ('0839899613932562','20',1,0);"
		);
		p->execSqlSync(
			"INSERT OR IGNORE INTO user_campaign_state (user_id)"
			" VALUES ('0839899613932562');"
		);
	});

	// Restrict the campaign mission catalog to the very first three story missions
	// (1, 2, 3).  Hypothesis: the client replays an "area-unlock" cutscene for
	// every newly-available mission it sees on a fresh session, and seeding the
	// full 1-200 list was triggering a cutscene cascade because EVERY mission
	// looked "newly unlocked" to the client.
	//
	// HYPOTHESIS RESULT: WRONG.  The client kept playing all area-open cutscenes
	// regardless of how many missions were in PermitPlace, AND additionally
	// crashed on second Grand Gaia entry because lands/areas/dungeons remained
	// broadly unlocked while their mission lists became empty (the world-map
	// renderer does not tolerate this inconsistency).  Migration is kept in
	// the history (it ran on existing DBs) but is paired with the recovery
	// migration below that restores the full mission catalog.
	migrate("06052026_RestrictToFirstThreeMissions", {
		// Wipe the broad seed from earlier migrations.
		p->execSqlSync(
			"DELETE FROM user_campaign_missions WHERE user_id='0839899613932562';"
		);
		// Seed only the first three missions as available.
		p->execSqlSync(
			"INSERT OR IGNORE INTO user_campaign_missions"
			" (user_id, mission_id, state, attain_percent)"
			" VALUES"
			" ('0839899613932562','1',1,0),"
			" ('0839899613932562','2',1,0),"
			" ('0839899613932562','3',1,0);"
		);
		// Ensure the per-user campaign state row exists for active-mission tracking.
		p->execSqlSync(
			"INSERT OR IGNORE INTO user_campaign_state (user_id)"
			" VALUES ('0839899613932562');"
		);
	});

	// Recovery migration paired with 06052026_RestrictToFirstThreeMissions.
	// Re-seeds missions 1-50 as available so the world map's mission list is
	// non-empty under every land/area the player has access to.  Restores the
	// pre-restriction stable state without requiring the player to delete
	// gme.sqlite manually.  INSERT OR IGNORE means already-cleared missions
	// keep their existing state (state=2, attain_percent=100).
	migrate("07052026_RestoreMissionCatalog", {
		p->execSqlSync(
			"INSERT OR IGNORE INTO user_campaign_missions"
			" (user_id, mission_id, state, attain_percent)"
			" VALUES"
			" ('0839899613932562','1',1,0),('0839899613932562','2',1,0),"
			" ('0839899613932562','3',1,0),('0839899613932562','4',1,0),"
			" ('0839899613932562','5',1,0),('0839899613932562','6',1,0),"
			" ('0839899613932562','7',1,0),('0839899613932562','8',1,0),"
			" ('0839899613932562','9',1,0),('0839899613932562','10',1,0),"
			" ('0839899613932562','11',1,0),('0839899613932562','12',1,0),"
			" ('0839899613932562','13',1,0),('0839899613932562','14',1,0),"
			" ('0839899613932562','15',1,0),('0839899613932562','16',1,0),"
			" ('0839899613932562','17',1,0),('0839899613932562','18',1,0),"
			" ('0839899613932562','19',1,0),('0839899613932562','20',1,0),"
			" ('0839899613932562','21',1,0),('0839899613932562','22',1,0),"
			" ('0839899613932562','23',1,0),('0839899613932562','24',1,0),"
			" ('0839899613932562','25',1,0),('0839899613932562','26',1,0),"
			" ('0839899613932562','27',1,0),('0839899613932562','28',1,0),"
			" ('0839899613932562','29',1,0),('0839899613932562','30',1,0),"
			" ('0839899613932562','31',1,0),('0839899613932562','32',1,0),"
			" ('0839899613932562','33',1,0),('0839899613932562','34',1,0),"
			" ('0839899613932562','35',1,0),('0839899613932562','36',1,0),"
			" ('0839899613932562','37',1,0),('0839899613932562','38',1,0),"
			" ('0839899613932562','39',1,0),('0839899613932562','40',1,0),"
			" ('0839899613932562','41',1,0),('0839899613932562','42',1,0),"
			" ('0839899613932562','43',1,0),('0839899613932562','44',1,0),"
			" ('0839899613932562','45',1,0),('0839899613932562','46',1,0),"
			" ('0839899613932562','47',1,0),('0839899613932562','48',1,0),"
			" ('0839899613932562','49',1,0),('0839899613932562','50',1,0);"
		);
	});

	// Realign campaign mission catalogue to the same three IDs PermitPlace now
	// emits (10, 11, 12 — the first three real Grand Gaia story missions, as
	// confirmed by http_log_jE6Sp0q4_*.log captures of the client's
	// MissionStart requests for the first/second/third campaign tiles).
	//
	// Hypothesis: the cutscene cascade fires once per "newly-visible mission"
	// the client sees, so by limiting the catalogue from ~50 down to 3 the
	// client should fire at most 3 cutscenes, and zero on subsequent entries
	// once those scenarios have been viewed locally.
	//
	// Earlier migration 06052026 tried this with mission IDs {1, 2, 3} and
	// crashed the client on second entry — those IDs are likely tutorial-only
	// content that the world-map renderer can't paint into Grand Gaia's
	// chapter-1 area.  Real story IDs (10/11/12) should be safe.
	migrate("07052026_RestrictToStoryMissions10_11_12", {
		// Wipe everything for our user — including the broad 1-50 seed from
		// the recovery migration above — so the campaign menu shows exactly
		// the same three tiles the world map allows.
		p->execSqlSync(
			"DELETE FROM user_campaign_missions WHERE user_id='0839899613932562';"
		);
		p->execSqlSync(
			"INSERT OR IGNORE INTO user_campaign_missions"
			" (user_id, mission_id, state, attain_percent)"
			" VALUES"
			" ('0839899613932562','10',1,0),"
			" ('0839899613932562','11',1,0),"
			" ('0839899613932562','12',1,0);"
		);
	});

	// Add a friend_point column to userinfo.  The wire UserTeamInfo carries
	// `friend_point` at key J3stQ7jd (used by the Honor Summon door's 200-FP
	// cost), but until now there was no DB column to back it — UserInfo and
	// GachaList both left ti.friend_point at the zero-initialised default.
	migrate("13052026_AddFriendPointColumn", {
		p->execSqlSync(
			"ALTER TABLE userinfo ADD COLUMN friend_point INTEGER NOT NULL DEFAULT 0;"
		);
	});

	// Top up testing currencies on the seed user so the summon flow has
	// values to spend.  Caps were chosen empirically — see handbook §10
	// ("Client currency display limits"):
	//   - paid_gems / free_gems: 9999 each.  The userinfo column hint is
	//     INTEGER(4) and the prior 95 000 seed silently overflowed the
	//     client's gem counter to 0 (see migration 27042026_SetGemsTo4200
	//     for the original incident).  9999 + 9999 = 19 998 total gems,
	//     enough for ~3 999 Rare-Summon pulls at 5 gems each.
	//   - friend_point: 9999.  Honor Summon costs 200 FP/pull, so this
	//     gives ~49 pulls before refill.  Real BF caps FP higher, but 9999
	//     stays in the same 4-digit safe band as gems.
	//   - summon_tickets / rainbow_coins / colosseum_tickets: 99 each.
	//     Tickets render in narrow 2-3 digit UI counters; 99 is the safe
	//     ceiling.
	//   - brave_coin: 9999.  Brave Coin is a separate currency from gems
	//     (despite both being keyed at "03UGMHxF" — gems on the door cost
	//     field, brave_coin in team_info).  Same 4-digit safe band.
	// Adjust here when the real client caps are pinned down; document any
	// observed overflow in the handbook §10 table.
	migrate("13052026_BumpCurrencyForSummonTesting", {
		p->execSqlSync(
			"UPDATE userinfo SET"
			" paid_gems        = 9999,"
			" free_gems        = 9999,"
			" friend_point     = 9999,"
			" summon_tickets   = 99,"
			" rainbow_coins    = 99,"
			" colosseum_tickets= 99,"
			" brave_coin       = 9999"
			" WHERE id='0839899613932562';"
		);
	});

	// V2 typed-ticket inventory.  Each row is "user_id owns `count` of ticket
	// type `ticket_id`".  UserInfo emits one `a3d5d12i` array entry per ticket
	// (so `count` entries per row); GachaAction with `324b023k`=1 against an
	// eligible door decrements the matching row.  Ticket-id → door mapping
	// lives in deploy/system/summon_tickets_v2.json (SummonTicketV2Mst at wire
	// key hE1d083b).  The seed grants 5 Brave Summon Tickets (id=8 → door
	// 17160 / Veteran summon's Brave half) so the ticket UI button surfaces
	// for testing.  See handbook §7.10.
	migrate("13052026_CreateUserSummonTicketsV2", {
		p->execSqlSync(
			"CREATE TABLE IF NOT EXISTS user_summon_tickets_v2 ("
			"user_id   TEXT    NOT NULL,"
			"ticket_id INTEGER NOT NULL,"
			"count     INTEGER NOT NULL DEFAULT 0,"
			"PRIMARY KEY (user_id, ticket_id)"
			");"
		);
		p->execSqlSync(
			"INSERT OR REPLACE INTO user_summon_tickets_v2 (user_id, ticket_id, count)"
			" VALUES ('0839899613932562', 8, 5);"
		);
	});

	// One-shot reset for DBs corrupted by the earlier MissionEnd level-up
	// loop that treated user_level.json d96tuT2E values as cumulative
	// thresholds.  See gimuserver/gme/MissionEnd.cpp comments and
	// mst/user_level.kdl `exp` doc for the full writeup.
	//
	// HISTORICAL: this migration originally set exp=0, based on a
	// misreading of d96tuT2E as a per-level chunk.  d96tuT2E is actually
	// a CUMULATIVE threshold (seed exp=1009680 IS lv 900's threshold,
	// not the lv-900 chunk).  Zeroing exp made the experience bar look
	// almost empty at login when it should be sitting right at the lv-900
	// baseline.  The follow-up migration below restores the correct seed.
	migrate("13052026_ResetLevelExpToCleanSeed", {
		p->execSqlSync(
			"UPDATE userinfo"
			" SET level = 900, exp = 0"
			" WHERE id = '0839899613932562';"
		);
	});

	// Restore the seed userinfo.exp to lv 900's cumulative threshold so
	// the player starts exactly at the lv 900 baseline (0 progress toward
	// lv 901).  See mst/user_level.kdl `exp` doc — d96tuT2E is cumulative,
	// so the value 1009680 corresponds to "you are at lv 900 with 0
	// progress; you need 173 more exp to reach lv 901".  The previous
	// migration zeroed this in error.
	//
	// HISTORICAL NOTE: this migration was written under the cumulative
	// interpretation of d96tuT2E.  The interpretation got flipped back
	// to per-level chunk in the follow-up migration below — see that
	// migration and mst/user_level.kdl `exp` doc for the final reading.
	migrate("13052026_RestoreSeedExpToLv900Threshold", {
		p->execSqlSync(
			"UPDATE userinfo"
			" SET level = 900, exp = 1009680"
			" WHERE id = '0839899613932562';"
		);
	});

	// Final reset to clean per-level-chunk baseline.  d96tuT2E in
	// user_level.json is per-level chunk; userinfo.exp is progress at the
	// current level (subtracted on level-up).  Seed values: level=900,
	// exp=1009680 means "1009680 progress at lv 900 toward the 1009853
	// chunk needed to ding lv 901" — bar reads ~99% full at login.
	//
	// Any DB that drifted into a higher level under the cumulative-style
	// MissionEnd loop (which never subtracted) ends up with exp values
	// that are absurd under per-level semantics (e.g. exp=1009880 at
	// lv 901, which would be "almost ready to ding 902" but in fact came
	// from cumulative carry-over).  Reset all such DBs to the clean
	// baseline.  Offline single-player — no legit progression lost.
	migrate("14052026_ResetToPerLevelChunkBaseline", {
		p->execSqlSync(
			"UPDATE userinfo"
			" SET level = 900, exp = 1009680"
			" WHERE id = '0839899613932562';"
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

