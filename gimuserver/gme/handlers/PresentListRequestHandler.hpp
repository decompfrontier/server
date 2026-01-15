#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class PresentListRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "nhjvB52R"; }
	const char* GetAesKey() const override { return "6F9sMzBxEv8jXpau"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
