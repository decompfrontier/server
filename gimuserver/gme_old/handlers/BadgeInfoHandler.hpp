#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class BadgeInfoHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "nJ3A7qFp"; }
	const char* GetAesKey() const override { return "bGxX67KB"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
