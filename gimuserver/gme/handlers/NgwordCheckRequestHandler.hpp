#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class NgwordCheckRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "TA4MnZX8"; }
	const char* GetAesKey() const override { return "r4Smw5TX"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
