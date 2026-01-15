#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class GetAchievementInfoRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "YPBU7MD8"; }
	const char* GetAesKey() const override { return "AKjzyZ81"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
