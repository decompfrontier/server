#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class RaidAgreementRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "Cu94xU8j"; }
	const char* GetAesKey() const override { return "Eh5a3yr9"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
