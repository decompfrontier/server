#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class DailyTaskUserInfoRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "m7g0Ekb5"; }
	const char* GetAesKey() const override { return "Hd8c3Y6"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
