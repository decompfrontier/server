#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class RaidUpDataUserReadyRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "vmdA2GD8"; }
	const char* GetAesKey() const override { return "esR7NZL9"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
