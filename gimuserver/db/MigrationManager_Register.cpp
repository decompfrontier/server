#include "MigrationManager.hpp"
#include "migrations/CreateDefaultTables.hpp"

#define ADD(x) m_migs.push_back(std::make_shared<Migrations::x>());

void MigrationManager::Register()
{
	ADD(CreateDefaultTables);
}
