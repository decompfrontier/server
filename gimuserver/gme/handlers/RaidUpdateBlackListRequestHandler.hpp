#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class RaidUpdateBlackListRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "4igfcyIm"; }
	const char* GetAesKey() const override { return "XIgQ4UHm"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
