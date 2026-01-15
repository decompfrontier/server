#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class MinigameNameEntryRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "oT8OP9Bp"; }
	const char* GetAesKey() const override { return "0xzd3AlK"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
