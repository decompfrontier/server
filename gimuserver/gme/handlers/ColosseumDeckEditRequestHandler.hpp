#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class ColosseumDeckEditRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "rdZbaKBG"; }
	const char* GetAesKey() const override { return "gY3YK5bD"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
