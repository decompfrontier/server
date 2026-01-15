#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class RaidUpDataRoomInfoRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "52H0dkjv"; }
	const char* GetAesKey() const override { return "9KdeuMX8"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
