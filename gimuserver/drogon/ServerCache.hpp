#pragma once

#include "ServerConfig.hpp"

/*!
* Cache of the server
*/
class ServerCache final : public trantor::NonCopyable
{
public:
	/*!
	* Setup the server cache.
	* @param[in] serverObj Configuration object of the plugin
	*/
	void Setup(const Json::Value& serverObj);

	/*!
	* Gets the banner configuration (dls).
	* @return DLS string
	*/
	inline const auto& dls() const { return m_dls; }

	/*!
	* Gets the server feature configuration.
	* @return Feature string
	*/
	inline const auto& feature() const { return m_feature; }

	/*!
	* Gets the common portion of the initialize response.
	* @return Initialize respose
	*/
	inline const auto& initializeResp() const { return m_initrsp;  }

	/*!
	* Gets the common portion of the user info response.
	* @return User info response
	*/
	inline const auto& userInfoResp() const { return m_userrsp; }

	/*!
	* Gets the cached summon list response.
	* @return GachaList response
	*/
	inline const auto& gachaListRsp() const { return m_gachaListRsp; }

	/*!
	* Gets the cached slot response.
	* @return ControlCenter response
	*/
	inline const auto& braveSlotsResp() const { return m_controlCenterRsp; }

	/*!
	* Server config.
	* @return Server config
	*/
	inline const auto& serverConfig() const { return m_serverConfig; }

	/*!
	* Unit master data (F_UNIT_MST). Empty until deploy/system/unit_mst.json
	* (hashed-key format, wrapper key "2r9cNSdt") is added and the loader
	* in ServerCache::Setup is uncommented.
	* @return Vector of UnitMst entries
	*/
	inline const auto& unitMst() const { return m_unitMst; }

	/*!
	* Mission master data (F_MISSION_MST_1 + F_MISSION_MST_2 merged).
	* Wire wrapper key "oXeC1Ak9".  Server uses this to look up per-mission
	* energy cost (69vnphig), exp reward (d96tuT2E), zel/karma rewards
	* (Rs7bCE3t/HTVh8a65), battle group ids (8f4NYKxb), etc. at handler
	* time so MissionStart no longer hardcodes -10 energy and MissionEnd
	* no longer fakes 100 exp / 500 zel / 100 karma.  See mst/mission.kdl
	* for the full field map.
	* @return Vector of MissionMst entries (3433 rows from the 21900 game data)
	*/
	inline const auto& missionMst() const { return m_missionMst; }

private:
	/*!
	* DLS cached JSON.
	*/
	std::string m_dls;

	/*!
	* Cached data of response.
	*/
	FeatureCheck m_feature{};

	/*!
	* Cached common data of the Initialize response
	*/
	InitializeResp m_initrsp{};

	/*!
	* Cached slot response
	*/
	SlotGameInfoR m_controlCenterRsp{};

	/*!
	* Server configuration.
	*/
	ServerConfig m_serverConfig;

	/*!
	* User info response.
	*/
	UserInfoResp m_userrsp{};

	/*!
	* Summon list response.
	*/
	GachaListResp m_gachaListRsp{};

	/*!
	* Unit master data, keyed/iterated by Unit handler ports.
	*/
	std::vector<UnitMst> m_unitMst;

	/*!
	* Mission master data (wrapper key "oXeC1Ak9"), looked up by mission_id
	* in MissionStart / MissionEnd.  Loaded from deploy/system/mission_mst.json.
	*/
	std::vector<MissionMst> m_missionMst;
};
