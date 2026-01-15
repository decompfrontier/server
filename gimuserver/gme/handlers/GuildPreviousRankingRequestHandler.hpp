#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class GuildPreviousRankingRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "oRa3ztp8"; }
	const char* GetAesKey() const override { return "pVg9L9Uw"; }
	// TODO: Response has 5 parameters to implement
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
