#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class FeSkillGetRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "nSQxNOeL"; }
	const char* GetAesKey() const override { return "nZ2bVoWu"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
