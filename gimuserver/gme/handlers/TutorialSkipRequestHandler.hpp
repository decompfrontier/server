#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class TutorialSkipRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "d36DaiJl"; }
	const char* GetAesKey() const override { return "p3qD61db"; }
	// TODO: Response has 2 parameters to implement
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
