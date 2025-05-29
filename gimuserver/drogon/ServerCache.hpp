#pragma once

#include <gimuserver/packets/net_handlers.hpp>
#include <gimuserver/packets/net_featurecheck.hpp>

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
	* Gets the cached slot response.
	* @return ControlCenter response
	*/
	inline const auto& braveSlotsResp() const { return m_controlCenterRsp; }

private:
	/*!
	* DLS cached JSON.
	*/
	std::string m_dls;

	/*!
	* Cached data of response.
	*/
	FeatureCheck m_feature;

	/*!
	* Cached common data of the Initialize response
	*/
	InitializeResp m_initrsp;

	/*!
	* Cached slot response
	*/
	SlotGameInfoR m_controlCenterRsp;
};
