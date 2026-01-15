#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class GuildDeckActivateRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "bk30i39b"; }
	const char* GetAesKey() const override { return "3bkb98a1"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
