#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class MinigameStartRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "SR7tf9pa"; }
	const char* GetAesKey() const override { return "XBSu90iF"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
