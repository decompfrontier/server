#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class MissionStartHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "jE6Sp0q4"; }
	const char* GetAesKey() const override { return "csiVLDKkxEwBfR70"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
