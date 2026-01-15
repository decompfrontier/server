#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class SummonerSkillGetRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "qWkYyw5i"; }
	const char* GetAesKey() const override { return "mrXjLLJB"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
