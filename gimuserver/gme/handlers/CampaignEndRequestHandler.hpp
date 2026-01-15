#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class CampaignEndRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "jF9Kkro4"; }
	const char* GetAesKey() const override { return "4X9tBSg8"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
