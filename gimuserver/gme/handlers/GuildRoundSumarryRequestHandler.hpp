#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class GuildRoundSumarryRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "Xfpo7jE2"; }
	const char* GetAesKey() const override { return "tVBMO5GW"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
