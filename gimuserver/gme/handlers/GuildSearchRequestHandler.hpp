#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class GuildSearchRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "R38ba9M3"; }
	const char* GetAesKey() const override { return "0D18dQn4"; }
	// TODO: Response has 9 parameters to implement
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
