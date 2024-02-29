#pragma once

#include <json/json.h>
#include "DailyTaskConfig.hpp"
#include "StartInfo.hpp"

class MstConfig
{
public:
	void LoadAllTables(const std::string& basePath);
	void CopyTo(Json::Value& v) const;

	const auto& DailyTaskConfig() const { return m_dailyTask; }
	const auto& StartInfo() const { return m_startInfo; }

protected:
	Json::Value m_convMst;
	::DailyTaskConfig m_dailyTask;
	::StartInfo m_startInfo;
};

