#pragma once


#include <json/value.h>

class LoginCampaign
{
	struct Table
	{
		std::string Name;
		std::string Description;
		unsigned int Version;
		unsigned int Unknown;
		unsigned int SubVersion;
	};

public:
	bool LoadTableFromJson(const std::string& path);
	void CopyTo(Json::Value& v) const;

protected:
	Json::Value m_convMst;
};
