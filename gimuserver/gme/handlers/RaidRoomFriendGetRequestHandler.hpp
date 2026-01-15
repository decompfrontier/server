#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class RaidRoomFriendGetRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "Xe2UB3A8"; }
	const char* GetAesKey() const override { return "9WN8oYBx"; }
	// TODO: Response has 48 parameters to implement
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
