#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class FGPlusGuildRankingRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "7umuchUg"; }
	const char* GetAesKey() const override { return "sWup7ehu"; }
	// TODO: Response has 4 parameters to implement
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
