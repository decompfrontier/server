#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class TutorialUpdateRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "T1nCVvx4"; }
	const char* GetAesKey() const override { return "7hqzmR3T"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
