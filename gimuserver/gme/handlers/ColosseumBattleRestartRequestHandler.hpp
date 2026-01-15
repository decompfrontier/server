#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class ColosseumBattleRestartRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "ohH9lfDE"; }
	const char* GetAesKey() const override { return "j2bFgyri"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
