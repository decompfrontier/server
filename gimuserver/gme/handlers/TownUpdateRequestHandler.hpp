#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class TownUpdateRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "CuQ5oB8U"; }
	const char* GetAesKey() const override { return "w1eo2ZDJ"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
