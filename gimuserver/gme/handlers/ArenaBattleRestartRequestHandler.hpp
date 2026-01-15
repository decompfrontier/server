#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class ArenaBattleRestartRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "NPyXDgV9"; }
	const char* GetAesKey() const override { return "sWofKGMl"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
