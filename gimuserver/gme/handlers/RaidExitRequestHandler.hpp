#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class RaidExitRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "jqi71nWu"; }
	const char* GetAesKey() const override { return "PD4ke3Ii"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
