#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class GuildDeckEditRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "adk28bij"; }
	const char* GetAesKey() const override { return "fgd3uu2b"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
