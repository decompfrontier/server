#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class GuildGuardianSummonContributeRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "yDDcC0vW"; }
	const char* GetAesKey() const override { return "NOeugPyv"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
