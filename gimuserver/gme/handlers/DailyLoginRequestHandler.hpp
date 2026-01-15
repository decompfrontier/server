#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class DailyLoginRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "4aClzokO"; }
	const char* GetAesKey() const override { return "stI81haQ"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
