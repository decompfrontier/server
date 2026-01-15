#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class RaidUpScenarioInfoRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "R38qvphm"; }
	const char* GetAesKey() const override { return "72EyFbW8"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
