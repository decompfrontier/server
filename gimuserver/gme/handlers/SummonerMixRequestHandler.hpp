#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class SummonerMixRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "nv4d3O7F"; }
	const char* GetAesKey() const override { return "vM65bAB4"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
