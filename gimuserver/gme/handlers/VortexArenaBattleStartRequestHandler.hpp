#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class VortexArenaBattleStartRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "j3s0iNM1"; }
	const char* GetAesKey() const override { return "uTk8nBdi"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
