#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class EventTokenExchangePurchaseRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "SLf48fs0"; }
	const char* GetAesKey() const override { return "Odiel30s"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
