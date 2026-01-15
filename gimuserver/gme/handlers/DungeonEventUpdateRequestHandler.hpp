#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class DungeonEventUpdateRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "BjAt1D6b"; }
	const char* GetAesKey() const override { return "k5EiNe9x"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
