#include "System.hpp"
#include <json/json.h>
#include <fstream>
#include <drogon/HttpAppFramework.h>

bool System::LoadSystemConfig(const std::string& p)
{
	std::ifstream ifs;
	ifs.open(p);

	if (!ifs.is_open())
		return false;

	try
	{
		Json::CharReaderBuilder rb;
		JSONCPP_STRING jss;
		Json::Value root;
		if (!Json::parseFromStream(rb, ifs, &root, &jss))
			return false;

		const auto& sys = root["system"];
		const auto& srv = root["server"];

		m_serverCfg.ParseFromJson(srv);
		ParseSystemConfig(sys);
	}
	catch (const std::exception& ex)
	{
		return false;
	}

	if (!m_mstInfo.LoadTableFromJson(m_mstInfoPath))
		return false;

	return true;
}

void System::ParseSystemConfig(const Json::Value& v)
{
	m_contentRoot = v["content_root"].asCString();
	m_mstInfoPath = v["mstinfo_path"].asCString();
	m_dbPath = v["gme_sqlite_path"].asCString();
}

void System::RunMigrations()
{
	m_mg.RunMigrations("gme");
}

System System::m_sys;
