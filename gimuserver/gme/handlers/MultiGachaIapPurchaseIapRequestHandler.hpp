#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class MultiGachaIapPurchaseIapRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "jk17biw9"; }
	const char* GetAesKey() const override { return "gIT163bi"; }
	// TODO: Response has 5 parameters to implement
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
