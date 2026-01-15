#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class ArenaBattleStartRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "7f1rg92L"; }
	const char* GetAesKey() const override { return "zaMR9o0d"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
