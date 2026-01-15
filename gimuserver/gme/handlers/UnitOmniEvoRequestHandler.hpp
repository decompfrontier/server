#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class UnitOmniEvoRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "4Dk4spf9"; }
	const char* GetAesKey() const override { return "4s3lsODp"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
