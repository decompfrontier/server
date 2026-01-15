#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class ColosseumBattleStartRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "ysP6qYcy"; }
	const char* GetAesKey() const override { return "iFIHAT5V"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
