#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class ColosseumGetRewardInfoRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "jv0dI88o"; }
	const char* GetAesKey() const override { return "d7aPxZWw"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
