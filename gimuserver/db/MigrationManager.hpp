#pragma once

#include "IMigration.hpp"

class MigrationManager
{
public:
	MigrationManager();

	void RunMigrations(const std::string& db);

protected:
	void Register();
	void CreateGetMigrationStatus(const std::string& db, std::vector<std::string>& hashes);

	std::vector<std::shared_ptr<IMigration>> m_migs;
};
