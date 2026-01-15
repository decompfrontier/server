#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class UserSummonerInfoEditRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "ZJYkXcHo"; }
	const char* GetAesKey() const override { return "lyR0us9b"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
