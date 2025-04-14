#include "MigrationManager.hpp"

MigrationManager::MigrationManager()
{
	Register();
}

void MigrationManager::CreateGetMigrationStatus(drogon::orm::DbClientPtr& p, std::vector<std::string>& hash)
{
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
			const auto& str = x[0].as<std::string>();
			LOG_DEBUG << "Migration found: " << str;
			hash.emplace_back(str);
		}
	}
}

void MigrationManager::RunMigrations(drogon::orm::DbClientPtr& ptr)
{
	std::vector<std::string> hash;
	CreateGetMigrationStatus(ptr, hash);

	for (const auto& p : m_migs)
	{
		if (std::find(hash.begin(), hash.end(), p->getName()) != hash.end())
			continue;

		LOG_INFO << "Execute migration " << p->getName();
		p->execute(ptr);
		ptr->execSqlSync("INSERT INTO migration_status(hash) VALUES ($1);", p->getName());
	}
}

