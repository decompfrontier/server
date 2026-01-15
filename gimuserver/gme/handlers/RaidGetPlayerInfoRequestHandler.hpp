#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class RaidGetPlayerInfoRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "81zkMjfN"; }
	const char* GetAesKey() const override { return "V7M6Bph4"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
