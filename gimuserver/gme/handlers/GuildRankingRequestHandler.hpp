#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class GuildRankingRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "2b9D01b4"; }
	const char* GetAesKey() const override { return "23Djab0e"; }
	// TODO: Response has 8 parameters to implement
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
