#pragma once

#include <string>
#include <drogon/orm/DbClient.h>
#include <gimuserver/system/MstConfig.hpp>
#include <gimuserver/system/ServerConfig.hpp>
#include <gimuserver/system/LogConfig.hpp>
#include <gimuserver/db/MigrationManager.hpp>

class System final
{
public:
    static System& Instance() { return m_sys; }

    void LoadSystemConfig(const std::string& path);
    void RunMigrations(drogon::orm::DbClientPtr ptr);
    void DevValidateAndSeedUnits(drogon::orm::DbClientPtr ptr);

    // Queries the users table for the first real account and caches the ID.
    // Called once at startup after RunMigrations. GmeController reads this
    // via GetActiveUserID() instead of using any compile-time constant.
    void ResolveActiveUser(drogon::orm::DbClientPtr ptr);

    // Returns the cached user ID resolved by ResolveActiveUser().
    // Empty string if ResolveActiveUser() has not run yet or the DB has no users.
    const std::string& GetActiveUserID() const { return m_activeUserID; }
    void SetActiveUserID(const std::string& id) { m_activeUserID = id; }

    // "system"
    const auto& GetContentRootPath() const { return m_contentRoot; }
    const auto& GetDbPath()          const { return m_dbPath; }
    const auto& GetSessionTimeout()  const { return m_sessionTimeout; }
    // "mst"
    const auto& MstConfig()    const { return m_mstConfig; }
    // "server"
    const auto& ServerConfig() { return m_serverCfg; }
    // "log"
    const auto& LogConfig()    const { return m_logCfg; }

private:
    void ParseSystemConfig(const Json::Value& v);

    std::string m_contentRoot;
    std::string m_dbPath;
    size_t      m_sessionTimeout;
    std::string m_activeUserID;   // set by ResolveActiveUser()

    ::ServerConfig   m_serverCfg;
    ::MstConfig      m_mstConfig;
    ::LogConfig      m_logCfg;
    MigrationManager m_mg;

    static System m_sys;
};