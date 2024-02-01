#pragma once

#include <string>
#include <gimuserver/system/MstInfo.hpp>
#include <gimuserver/system/ServerConfig.hpp>

class System final
{
public:
	static System& Instance() { return m_sys; }

	bool LoadSystemConfig(const std::string& path);

	const auto& MstInfo() const { return m_mstInfo; }
	auto& ServerConfig() { return m_serverCfg; }
	const auto& GetContentRootPath() const { return m_contentRoot; }
	
private:
	void ParseSystemConfig(const Json::Value& v);

	std::string m_contentRoot;
	std::string m_mstInfoPath;

	::ServerConfig m_serverCfg;
	::MstInfo m_mstInfo;

	static System m_sys; // SINGLETON!!!!
};
