#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class RaidMissionUseItemRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "o03kCtb1"; }
	const char* GetAesKey() const override { return "Ys8ro9Sq"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
