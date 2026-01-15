#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class RaidQuickStartRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "CWeTY8BQ"; }
	const char* GetAesKey() const override { return "HsVofOJJ"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
