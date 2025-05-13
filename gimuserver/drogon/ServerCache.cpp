#include "App.hpp"
#include "ServerCache.hpp"

#include <gimuserver/packets/net_gumi_live.hpp>
#include <gimuserver/utils/BfCrypt.hpp>

/*!
* Builds a JSON
* @param[in] d Template class to write
* @return Parsed json string
*/
template <typename T>
static std::string BuildJson(const T& d)
{
	std::string buffer;
	const auto& ec = glz::write_json(d, buffer);
	if (ec.ec != glz::error_code::none)
	{
		LOG_DEBUG << "Cannot write internal json! please debug!";
		return "";
	}

	return buffer;
}

void ServerCache::Setup(const Json::Value& serverObj)
{
	GameDls dls;
	dls.game_ip = GetDrogonBindHostname();
	dls.resource_ip = dls.game_ip;
	dls.version = serverObj["game_version"].asUInt();
	dls.gumilive_ip = dls.game_ip + "/";
	dls.bg_image = serverObj["wallpaper_banner"].asString();

	const auto dlsJson = BuildJson(dls);
	const auto sree = BfCrypt::BuildSREE(dlsJson);

	// build SREE crypted JSON rather than constructing it everytime in the dls controller
	if (sree.has_value())
	{
		m_dls = BuildJson(sree.value());
	}
}
