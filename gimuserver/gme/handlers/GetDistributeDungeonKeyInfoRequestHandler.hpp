#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class GetDistributeDungeonKeyInfoRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "1mr9UsYz"; }
	const char* GetAesKey() const override { return "r0ZA3pn5"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
