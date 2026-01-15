#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class ItemUseTimeLimitRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "4eCLR4rq"; }
	const char* GetAesKey() const override { return "1O3NPf93"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
