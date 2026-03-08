#include "System.hpp"
#include <json/json.h>
#include <fstream>
#include <filesystem>
#include <drogon/HttpAppFramework.h>
#include <gimuserver/gumiapi/AccountController.hpp>

void System::LoadSystemConfig(const std::string& p)
{
    std::ifstream ifs;
    ifs.open(p);

    if (!ifs.is_open())
        throw std::runtime_error("gimuconfig.json: not found");

    Json::CharReaderBuilder rb;
    JSONCPP_STRING jss;
    Json::Value root;
    if (!Json::parseFromStream(rb, ifs, &root, &jss))
        throw std::runtime_error("gimuconfig.json: parse error");

    const auto& sys = root["system"];
    const auto& srv = root["server"];
    const auto& log = root["log"];

    m_serverCfg.ParseFromJson(srv);
    m_logCfg.ParseFromJson(log);

    ParseSystemConfig(sys);

    std::filesystem::create_directory(m_logCfg.RequestPath);
    std::filesystem::create_directory(m_logCfg.ResponsePath);
}

void System::ParseSystemConfig(const Json::Value& v)
{
    m_contentRoot = v["content_root"].asString();
    m_dbPath = v["gme_sqlite_path"].asString();
    m_sessionTimeout = size_t(v["session_timeout"].asUInt64());

    m_mstConfig.LoadAllTables(v["mst_root"].asString());
}

void System::RunMigrations(drogon::orm::DbClientPtr p)
{
    m_mg.RunMigrations(p);
}

void System::ResolveActiveUser(drogon::orm::DbClientPtr db)
{
    try
    {
        auto result = db->execSqlSync("SELECT id, username FROM users LIMIT 1;");
        if (!result.empty())
        {
            m_activeUserID = result[0]["id"].as<std::string>();
            std::string username = result[0]["username"].as<std::string>();
            LOG_INFO << "System::ResolveActiveUser: active user -> '"
                << username << "' (id=" << m_activeUserID << ")";
        }
        else
        {
            LOG_WARN << "System::ResolveActiveUser: no users in DB yet. "
                << "Launch the game client once to create an account, "
                << "then restart the server.";
        }
    }
    catch (const drogon::orm::DrogonDbException& e)
    {
        LOG_ERROR << "System::ResolveActiveUser: DB error: " << e.base().what();
    }
}

void System::DevValidateAndSeedUnits(drogon::orm::DbClientPtr db)
{
#if !DEV_SKIP_TUTORIAL
    return;
#else
    const size_t expectedCount = ALL_UNIT_IDS.size();

    LOG_INFO << "System::DevValidateAndSeedUnits: checking existing users "
        << "(expecting " << expectedCount << " units each)";

    try
    {
        auto users = db->execSqlSync("SELECT id FROM users;");

        if (users.empty())
        {
            LOG_INFO << "System::DevValidateAndSeedUnits: no existing users, nothing to do.";
            return;
        }

        for (const auto& row : users)
        {
            const std::string userId = row["id"].as<std::string>();

            auto countResult = db->execSqlSync(
                "SELECT COUNT(*) FROM user_units WHERE user_id = $1;", userId);

            const size_t have = countResult[0][0].as<size_t>();

            if (have >= expectedCount)
            {
                LOG_DEBUG << "System::DevValidateAndSeedUnits: user " << userId
                    << " already has " << have << " units, skipping.";
                continue;
            }

            LOG_INFO << "System::DevValidateAndSeedUnits: user " << userId
                << " has " << have << "/" << expectedCount << " units - seeding.";

            for (const auto& unitId : ALL_UNIT_IDS)
            {
                try
                {
                    db->execSqlSync(
                        "INSERT OR IGNORE INTO user_units (user_id, unit_id) VALUES ($1, $2);",
                        userId, unitId);
                }
                catch (const drogon::orm::DrogonDbException& e)
                {
                    LOG_WARN << "System::DevValidateAndSeedUnits: failed to insert unit "
                        << unitId << " for " << userId << ": " << e.base().what();
                }
            }

            LOG_INFO << "System::DevValidateAndSeedUnits: finished seeding user " << userId;
        }
    }
    catch (const drogon::orm::DrogonDbException& e)
    {
        LOG_ERROR << "System::DevValidateAndSeedUnits: DB error: " << e.base().what();
    }
#endif
}

System System::m_sys;