#include "App.hpp"
#include "GimuServer.hpp"

GimuServer::GimuServer() : m_http_log(), m_dlc_error_log(), m_have_log(false), m_cache() {}

void GimuServer::initAndStart(const Json::Value& config)
{
	const auto& extralog = config["extra_log"];
	if (extralog["enable"].asBool())
	{
		const auto& dir = extralog["directory"].asString();
		const auto& httpLogName = extralog["http_log_name"].asString();
		const auto& dlcLogName = extralog["dlc_error_log_name"].asString();

#if 0
		m_http_log.setFileName(httpLogName.c_str(), ".log", dir.c_str());
		m_http_log.startLogging();

		m_dlc_error_log.setFileName(dlcLogName.c_str(), ".log", dir.c_str());
		m_dlc_error_log.startLogging();
#endif

		m_have_log = true;
	}

	const auto& server = config["server"];
	m_cache.Setup(server);
}

void GimuServer::shutdown() {}
