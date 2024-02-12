#pragma once

#include "../IMigration.hpp"
#include "../DbMacro.hpp"

MIGRATION_NS_BEGIN
struct CreateDefaultTables : public IMigration
{
	virtual void execute() override
	{
		auto& p = GME_DB;
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
			"avail_brave_points INTEGER(9)"
			");"
		);
	}

	const char* getName() const override { return "06022024_CreateUserInfoTable"; }
};
MIGRATION_NS_END