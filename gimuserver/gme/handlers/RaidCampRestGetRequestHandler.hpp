#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class RaidCampRestGetRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "diIe8ux7"; }
	const char* GetAesKey() const override { return "G5uind2r"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
