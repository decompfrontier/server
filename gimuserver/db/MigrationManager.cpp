#include "MigrationManager.hpp"

MigrationManager::MigrationManager()
{
	Register();
}

void MigrationManager::CreateGetMigrationStatus(const std::string& db, std::vector<std::string>& hash)
{
	auto& p = drogon::app().getFastDbClient(db);
	p->execSqlSync(
		"CREATE TABLE IF NOT EXISTS migration_status("
		"hash TEXT PRIMARY KEY"
		");"
	);
	auto res = p->execSqlSync(
		"SELECT hash FROM migration_status;"
	);
	if (res.size() > 0)
	{
		for (const auto& x : res)
		{
			hash.emplace_back(x[0].as<std::string>());
		}
	}
}

void MigrationManager::RunMigrations(const std::string& db)
{
	std::vector<std::string> hash;
	CreateGetMigrationStatus(db, hash);

	for (const auto& p : m_migs)
	{
		if (std::find(hash.begin(), hash.end(), p->getName()) != hash.end())
			continue;

		p->execute();
		drogon::app().getFastDbClient(db)->execSqlSync("INSERT INTO migration_status(hash) VALUES ($1);", p->getName());
	}
}
