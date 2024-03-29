#pragma once

#include <json/json.h>
#include "DailyTaskConfig.hpp"
#include "StartInfo.hpp"
#include <gimuserver/gme/response/VideoAdsSlotgameInfo.hpp>
#include <gimuserver/gme/response/UserLevelMst.hpp>

class MstConfig
{
public:
	void LoadAllTables(const std::string& basePath);
	void CopyInitializeMstTo(Json::Value& v) const
	{
		for (auto srcIt = m_initMst.begin(); srcIt != m_initMst.end(); ++srcIt)
			v[srcIt.name()] = *srcIt;
	}

	void CopyUserInfoMstTo(Json::Value& v) const
	{
		for (auto srcIt = m_userInfoMst.begin(); srcIt != m_userInfoMst.end(); ++srcIt)
			v[srcIt.name()] = *srcIt;
	}

	void CopyGachaInfoTo(Json::Value& v) const {
		for (auto srcIt = m_gatchaInfo.begin(); srcIt != m_gatchaInfo.end(); ++srcIt)
			v[srcIt.name()] = *srcIt;
	}

	const auto& DailyTaskConfig() const { return m_dailyTask; }
	const auto& StartInfo() const { return m_startInfo; }
	const auto& GetAdsSlotInfo() const { return m_videoAdsSlot; }
	const auto& GetProgressionInfo() const { return m_progressions; }
	

private:
	void LoadGacha(const std::string& path);
	void LoadExtraSkillPassive(const std::string& path);
	void LoadUrl(const std::string& path);
	void LoadReceipes(const std::string& path);
	void LoadProgressionInfo(const std::string& path);

	// cached jsons
	Json::Value m_initMst;
	Json::Value m_userInfoMst;
	Json::Value m_gatchaInfo;

	// caches classes
	Response::VideoAdsSlotgameInfo m_videoAdsSlot;
	Response::UserLevelMst m_progressions;
	::DailyTaskConfig m_dailyTask;
	::StartInfo m_startInfo;

};

