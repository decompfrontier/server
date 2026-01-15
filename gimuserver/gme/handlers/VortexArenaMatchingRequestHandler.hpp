#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class VortexArenaMatchingRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "b5TunM93"; }
	const char* GetAesKey() const override { return "a7GkyuV3"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
