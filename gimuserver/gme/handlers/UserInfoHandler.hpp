#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class UserInfoHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "cTZ3W2JG"; }
	const char* GetAesKey() const override { return "ScJx6ywWEb0A3njT"; }
	void Handle(const drogon::SessionPtr& session, DrogonCallback& cb, const Json::Value& req) const override;
};
HANDLER_NS_END
