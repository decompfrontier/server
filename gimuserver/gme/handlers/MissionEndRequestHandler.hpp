#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class MissionEndRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "9TvyNR5H"; }
	const char* GetAesKey() const override { return "oINq0rfUFPx5MgmT"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
