#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class RaidGetChatLogRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "k5pMN68B"; }
	const char* GetAesKey() const override { return "cy0oLna7"; }
	// TODO: Response has 8 parameters to implement
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
