#include "MigrationManager.hpp"
#include "migrations/CreateDefaultTables.hpp"
#include "migrations/CreateUserUnitsTable.hpp"

#define ADD(x) m_migs.push_back(std::make_shared<Migrations::##x>());

void MigrationManager::Register()
{
    ADD(CreateDefaultTables);
    ADD(CreateUserUnitsTable);
}