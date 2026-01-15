#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class CampaignRestartRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "Ht2jeWV8"; }
	const char* GetAesKey() const override { return "vm7LYZz4"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
