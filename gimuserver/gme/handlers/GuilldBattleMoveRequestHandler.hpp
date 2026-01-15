#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class GuilldBattleMoveRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "8b6DKiqz"; }
	const char* GetAesKey() const override { return "b8akzyk1"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
