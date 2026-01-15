#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class RaidLimitedUseItemRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "SaN7v13E"; }
	const char* GetAesKey() const override { return "btR9Jkw7"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
