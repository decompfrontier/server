#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class SetPurchaseInfoRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "u06y2UE4"; }
	const char* GetAesKey() const override { return "c2kjaKv8"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
