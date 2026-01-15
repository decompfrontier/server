#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class RaidCampRestStartRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "I7nVUv0h"; }
	const char* GetAesKey() const override { return "qy27SYu9"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
