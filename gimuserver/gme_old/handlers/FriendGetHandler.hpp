#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class FriendGetHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "2o4axPIC"; }
	const char* GetAesKey() const override { return "EoYuZ2nbImhCU1c0"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
