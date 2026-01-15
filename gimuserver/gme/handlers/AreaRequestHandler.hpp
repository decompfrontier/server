#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class AreaRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "Zds63G5y"; }
	const char* GetAesKey() const override { return "YfAh7gqojdXEtFR1"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
