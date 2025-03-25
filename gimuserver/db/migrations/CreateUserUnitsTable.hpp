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
            "id INTEGER PRIMARY KEY AUTOINCREMENT," // Add: Auto-incrementing primary key as per PR comment
            "user_id TEXT NOT NULL," // Keep: Links unit to a user
            "unit_id TEXT NOT NULL" // Keep: Stores the unit identifier
            ");"
        );
    }

    const char* getName() const override { return "08032025_CreateUserUnitsTable"; } // Unique migration name with date
};
MIGRATION_NS_END