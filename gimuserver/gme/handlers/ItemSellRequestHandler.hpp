#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class ItemSellRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "qDQerU74"; }
	const char* GetAesKey() const override { return "73aFNjPu"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
