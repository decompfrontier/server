#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class RaidItemDeleteRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "N43JmIf2"; }
	const char* GetAesKey() const override { return "jvm3q24f"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
