#pragma once

#include <string>
#include <gimuserver/system/MstInfo.hpp>
#include <gimuserver/system/ServerConfig.hpp>
#include <gimuserver/db/MigrationManager.hpp>

#include <drogon/orm/DbClient.h>

class System final
{
public:
	static System& Instance() { return m_sys; }

	bool LoadSystemConfig(const std::string& path);
	void RunMigrations();

	const auto& MstInfo() const { return m_mstInfo; }
	auto& ServerConfig() { return m_serverCfg; }
	const auto& GetContentRootPath() const { return m_contentRoot; }
	const auto& GetDbPath() const { return m_dbPath; }

private:
	void ParseSystemConfig(const Json::Value& v);

	std::string m_contentRoot;
	std::string m_mstInfoPath;
	std::string m_dbPath;

	::ServerConfig m_serverCfg;
	::MstInfo m_mstInfo;

	MigrationManager m_mg;

	static System m_sys; // SINGLETON!!!!
};
