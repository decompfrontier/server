#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class GuildInfoRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "138ba8d4"; }
	const char* GetAesKey() const override { return "23gD81ia"; }
	// TODO: Response has 12 parameters to implement
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
