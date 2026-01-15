#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class RaidRoomInfoRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "0VxE6kTW"; }
	const char* GetAesKey() const override { return "cUrw7K6W"; }
	// TODO: Response has 38 parameters to implement
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
