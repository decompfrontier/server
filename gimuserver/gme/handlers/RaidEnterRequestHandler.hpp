#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class RaidEnterRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "yZReG75i"; }
	const char* GetAesKey() const override { return "N0Azw6ob"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
