#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class UserEnteredFeatureRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "983D5Dii"; }
	const char* GetAesKey() const override { return "Dr6pwV3i"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
