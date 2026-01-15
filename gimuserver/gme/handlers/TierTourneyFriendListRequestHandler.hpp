#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class TierTourneyFriendListRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "p3iaD81y"; }
	const char* GetAesKey() const override { return "B1oeKLqUw03wELoz"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
