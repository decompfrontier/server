#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class ColosseumRewardReceiptRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "PN5CVaWy"; }
	const char* GetAesKey() const override { return "YBgVBy5A"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
