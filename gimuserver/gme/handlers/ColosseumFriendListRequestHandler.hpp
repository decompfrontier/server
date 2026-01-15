#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class ColosseumFriendListRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "AoBvG2YN"; }
	const char* GetAesKey() const override { return "Wj5fH1HV"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
