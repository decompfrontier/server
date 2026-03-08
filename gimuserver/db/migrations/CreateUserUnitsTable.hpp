#pragma once

#include "../IMigration.hpp"
#include "../DbMacro.hpp"

MIGRATION_NS_BEGIN

struct CreateUserUnitsTable : public IMigration
{
    void execute(drogon::orm::DbClientPtr db) override
    {
        db->execSqlSync(R"(
            CREATE TABLE IF NOT EXISTS user_units (
                id       INTEGER PRIMARY KEY AUTOINCREMENT,
                user_id  TEXT NOT NULL,
                unit_id  TEXT NOT NULL,
                UNIQUE(user_id, unit_id)
            );
        )");

        // The game client uses user_units.id as the in-game unit instance
        // identifier. IDs starting from 1 cause a client crash — the safe
        // minimum observed in captured traffic is ~10407.
        // Inserting and immediately deleting a sentinel row bumps the
        // AUTOINCREMENT sequence to 10000 so all seeded units get ids >= 10001.
        db->execSqlSync(
            "INSERT INTO user_units (id, user_id, unit_id) VALUES (10000, '__sentinel__', '__sentinel__');"
        );
        db->execSqlSync(
            "DELETE FROM user_units WHERE user_id = '__sentinel__';"
        );
    }

    const char* getName() const override { return "08032025_CreateUserUnitsTable"; }
};

MIGRATION_NS_END