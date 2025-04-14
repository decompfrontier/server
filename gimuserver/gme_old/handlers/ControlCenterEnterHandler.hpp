#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class ControlCenterEnterHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "uYF93Mhc"; }
	const char* GetAesKey() const override { return "d0k6LGUu"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
