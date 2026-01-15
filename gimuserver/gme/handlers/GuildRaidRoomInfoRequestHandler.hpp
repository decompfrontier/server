#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class GuildRaidRoomInfoRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "83kBdiqD"; }
	const char* GetAesKey() const override { return "93Di3Ge8"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
