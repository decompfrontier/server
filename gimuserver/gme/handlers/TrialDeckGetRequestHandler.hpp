#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class TrialDeckGetRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "gY63Zvy7"; }
	const char* GetAesKey() const override { return "5QuLx4DG"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
