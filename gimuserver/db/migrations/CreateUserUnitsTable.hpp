#pragma once

#include "../IMigration.hpp"
#include "../DbMacro.hpp"

MIGRATION_NS_BEGIN
struct CreateUserUnitsTable : public IMigration
{
    virtual void execute(drogon::orm::DbClientPtr p) override
    {
        p->execSqlSync(
            "CREATE TABLE IF NOT EXISTS user_units ("
            "user_id TEXT NOT NULL,"
            "unit_id TEXT NOT NULL,"
            "name TEXT NOT NULL,"
            "rarity INTEGER NOT NULL,"
            "element TEXT NOT NULL,"
            "stats TEXT NOT NULL,"
            "bb TEXT NOT NULL,"
            "leader_skill TEXT,"
            "ai TEXT,"
            "data TEXT NOT NULL,"
            "PRIMARY KEY (user_id, unit_id)"
            ");"
        );
    }

    const char* getName() const override { return "08032025_CreateUserUnitsTable"; } // Unique migration name with date
};
MIGRATION_NS_END