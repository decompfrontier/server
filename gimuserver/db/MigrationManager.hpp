#pragma once

#include "IMigration.hpp"

class MigrationManager
{
public:
	MigrationManager();

	void RunMigrations(drogon::orm::DbClientPtr p);

protected:
	void Register();
	void CreateGetMigrationStatus(drogon::orm::DbClientPtr db, std::vector<std::string>& hashes);

	std::vector<std::shared_ptr<IMigration>> m_migs;
};
