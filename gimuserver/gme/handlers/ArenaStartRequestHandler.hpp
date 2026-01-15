#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class ArenaStartRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "zu6ifpy0"; }
	const char* GetAesKey() const override { return "bGjK4UH1"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
