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
		const auto& mst = root["mst"];

		if (!m_serverCfg.ParseFromJson(srv))
			return false;
		
		ParseSystemConfig(sys);

		if (!ParseMstConfig(mst))
			return false;
	}
	catch (const std::exception& ex)
	{
		return false;
	}

	return true;
}

void System::ParseSystemConfig(const Json::Value& v)
{
	m_contentRoot = v["content_root"].asCString();
	m_dbPath = v["gme_sqlite_path"].asCString();
	m_sessionTimeout = size_t(v["session_timeout"].asUInt64());
}

bool System::ParseMstConfig(const Json::Value& v)
{
	if (!m_mstConfig.Info.LoadTableFromJson(v["mstinfo_path"].asCString()))
		return false;

	if (!m_mstConfig.DailyTask.LoadTableFromJson(v["dailytask_path"].asCString()))
		return false;

	if (!m_mstConfig.LoginCamp.LoadTableFromJson(v["logincampaign_path"].asCString()))
		return false;

	return true;
}

void System::RunMigrations(drogon::orm::DbClientPtr p)
{
	m_mg.RunMigrations(p);
}

System System::m_sys;
