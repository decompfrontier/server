#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class AchievementAcceptRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "dx5qvm7L"; }
	const char* GetAesKey() const override { return "g9N1y7bc"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
