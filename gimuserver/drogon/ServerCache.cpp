#include "App.hpp"
#include "ServerCache.hpp"

#include <gimuserver/utils/BfCrypt.hpp>
#include <gimuserver/packets/net_gumi_live.hpp>
#include <gimuserver/packets/net_featurecheck.hpp>

/*!
* Builds a JSON
* @param[in] d Template class to write
* @return Parsed json string
*/
template <typename T>
static std::string BuildJson(const T& d)
{
	std::string buffer{};
	const auto& ec = glz::write_json(d, buffer);
	if (ec)
	{
		throw std::runtime_error("Cannot build a cache JSON, error:\n{}" + glz::format_error(ec, buffer));
	}

	return buffer;
}

/*!
* Loads a JSON from the file system.
*/
template <typename T>
static T LoadJson(std::string_view file)
{
	T obj{};
	auto path = getSysRoot() + std::string(file);
	std::string buffer{};
	const auto& ec = glz::read_file_json(obj, path, buffer);
	if (ec)
	{
		throw std::runtime_error(std::format("Cannot read JSON file \"{}\", error:\n{}", file, glz::format_error(ec, buffer)));
	}

	return obj;
}

void ServerCache::Setup(const Json::Value& serverObj)
{
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
		else
		{
			throw std::runtime_error("Cannot encrypt SREE cache json");
		}
	}
	{
		const auto& feature = LoadJson<FeatureCheck>("features.json");
		m_feature = BuildJson(feature);
	}
}
