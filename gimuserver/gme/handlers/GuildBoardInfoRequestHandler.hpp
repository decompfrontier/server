#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class GuildBoardInfoRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "J93ki3Bw"; }
	const char* GetAesKey() const override { return "m83D19ib"; }
	// TODO: Response has 9 parameters to implement
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
