#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class RaidRoonItemEditRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "bv9xt3m6"; }
	const char* GetAesKey() const override { return "9mKud8kc"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
