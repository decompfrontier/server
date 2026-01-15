#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class GuildTradeRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "38adiJeb"; }
	const char* GetAesKey() const override { return "ja3biAqb"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
