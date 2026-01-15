#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class GuildBoardPostRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "X89bDai1"; }
	const char* GetAesKey() const override { return "Bi9Ralbq"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
