#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class GuildCreateRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "g298Da10"; }
	const char* GetAesKey() const override { return "G23Bd01d"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
