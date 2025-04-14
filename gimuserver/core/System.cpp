#include "System.hpp"

#include <fstream>
#include <filesystem>
#include <drogon/HttpAppFramework.h>
#include <glaze_helpers.hpp>

void System::LoadSystemConfig(std::string_view p)
{	
	auto ec = glz::read_file_json(m_sysConfig, p, std::string{});
	if (ec)
		throw std::runtime_error("Cannt load system config: " + glz::format_error(ec));

	std::filesystem::create_directory(m_sysConfig.log.request_path);
	std::filesystem::create_directory(m_sysConfig.log.response_path);

	m_mstConfig.LoadAllTables(m_sysConfig.system.mst_root);
}

System System::m_sys;
