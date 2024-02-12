#include "LoginCampaign.hpp"
#include <json/json.h>
#include <fstream>
#include "gme/response/LoginCampaignMst.hpp"
#include "gme/response/LoginCampaignReward.hpp"

bool LoginCampaign::LoadTableFromJson(const std::string& path)
{
	std::ifstream ifs;
	ifs.open(path);

	if (!ifs.is_open())
		return false;

	try
	{
		Json::CharReaderBuilder rb;
		JSONCPP_STRING jss;
		Json::Value root;
		if (!Json::parseFromStream(rb, ifs, &root, &jss))
			return false;

		Response::LoginCampaignMst camp;

		camp.image = root["login_banner"].asCString();
		const std::string icon = root["login_icon"].asCString();
		camp.startDate = root["start_time"].asUInt();
		camp.totalDays = root["duration"].asUInt();
		const auto icon_days = root["banner_icon_days"].asUInt();

		std::string icon_prefix;
		std::string icon_suffix = "";

		auto p = icon.find('@');
		if (p == std::string::npos)
		{
			icon_prefix = icon;
		}
		else
		{
			icon_prefix = icon.substr(0, p);
			icon_suffix = icon.substr(p + 1);
		}

		Response::LoginCampaignReward rew;
		for (uint32_t i = 1; i <= icon_days; i++)
		{
			Response::LoginCampaignReward::Data d;
			d.id = camp.id;
			d.reward_day = i;
			d.reward_img = icon_prefix + std::to_string(i) + icon_suffix;
			rew.Mst.emplace_back(d);
		}

		Json::Value res;
		camp.Serialize(res);
		rew.Serialize(res);

		m_convMst = res;
	}
	catch (const std::exception& ex)
	{
		return false;
	}

	return true;
}

void LoginCampaign::CopyTo(Json::Value& v) const
{
	for (auto srcIt = m_convMst.begin(); srcIt != m_convMst.end(); ++srcIt)
		v[srcIt.name()] = *srcIt;
}
