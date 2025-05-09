#pragma once

#include "SystemConfig.hpp"

#include <gimuserver/system/MstCache.hpp>
#include <gimuserver/db/MigrationManager.hpp>

#include <drogon/orm/DbClient.h>

/*!
* Main instance of the server configuration
*/
class System final : public trantor::NonCopyable
{
public:
	/*!
	* Instance singleton
	*/
	static inline System& Instance() { return m_sys; }

	void LoadSystemConfig(std::string_view path);

	inline void RunMigrations(drogon::orm::DbClientPtr& ptr) { m_mg.RunMigrations(ptr); }
	inline const auto& MstConfig() const { return m_mstConfig; }
	inline const auto& SysConfig() const { return m_sysConfig;  }

private:
	/*!
	* Private constructor for singleton.
	*/
	System();

	/*!
	* Deleted copy constructor.
	*/
	System(const System&) = delete;

	/*!
	* Delete assignment constructor.
	*/
	System& operator=(const System&) = delete;

	/*!
	* System configuration instance.
	*/
	SystemConfig m_sysConfig;

	::MstConfig m_mstConfig;

	/*!
	* Migration manager.
	*/
	MigrationManager m_mg;

	/*!
	* Global singleton.
	*/
	static System m_sys;
};
