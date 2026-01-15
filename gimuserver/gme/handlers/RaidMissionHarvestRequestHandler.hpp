#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class RaidMissionHarvestRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "9QdRVwB1"; }
	const char* GetAesKey() const override { return "b1ScyFv9"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
