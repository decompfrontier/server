#pragma once

#include "SystemConfig.hpp"

#include <gimuserver/system/MstConfig.hpp>
#include <gimuserver/db/MigrationManager.hpp>

#include <drogon/orm/DbClient.h>

class System final
{
public:
	static inline System& Instance() { return m_sys; }

	void LoadSystemConfig(std::string_view path);

	inline void RunMigrations(drogon::orm::DbClientPtr ptr) { m_mg.RunMigrations(ptr); }
	inline const auto& MstConfig() const { return m_mstConfig; }
	inline const auto& SysConfig() const { return m_sysConfig;  }

private:
	SystemConfig m_sysConfig;
	::MstConfig m_mstConfig;
	MigrationManager m_mg;

	static System m_sys; // SINGLETON!!!!
};
