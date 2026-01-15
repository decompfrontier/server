#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class RaidMissionStartRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "2qDW9NBU"; }
	const char* GetAesKey() const override { return "kY5y1zBj"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
