#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class SPChallengeMissionStartRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "QZy7cSW1"; }
	const char* GetAesKey() const override { return "qojy81wY"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
