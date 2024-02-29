#include "System.hpp"
#include <json/json.h>
#include <fstream>
#include <filesystem>
#include <drogon/HttpAppFramework.h>

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

System System::m_sys;
