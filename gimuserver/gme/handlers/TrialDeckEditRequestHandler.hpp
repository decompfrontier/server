#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class TrialDeckEditRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "MbdL5D4K"; }
	const char* GetAesKey() const override { return "Bb1T0qdC"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
