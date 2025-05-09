#pragma once

#include <gimuserver/core/SystemConfig.hpp>

/*!
* Holds the cache for all JSON files loaded in the deploy directory
*/
class MstCache final
{
public:
	/**
	* Loads the cache files.
	* @param cfg Configuration file
	* @param basePath Root directory where the mst caches are found
	*/
	void Load(const SystemConfig& cfg, const std::string& basePath);

#if 0
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
#endif

private:
#if 0
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
	VideoAdsSlotGameInfo m_videoAdsSlot;
	UserLevelMst m_progressions;
	::DailyTaskConfig m_dailyTask;
	::StartInfo m_startInfo;
#endif
};

