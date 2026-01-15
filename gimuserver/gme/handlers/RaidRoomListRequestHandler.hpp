#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class RaidRoomListRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "YF9UQpn6"; }
	const char* GetAesKey() const override { return "c0Qf94Ir"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
