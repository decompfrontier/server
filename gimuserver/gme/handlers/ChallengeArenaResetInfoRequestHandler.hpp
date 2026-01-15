#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class ChallengeArenaResetInfoRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "Zw3WIoWu"; }
	const char* GetAesKey() const override { return "KlwYMGF1"; }
	// TODO: Response has 4 parameters to implement
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
