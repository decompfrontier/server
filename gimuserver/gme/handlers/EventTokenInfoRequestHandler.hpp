#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class EventTokenInfoRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "f49als4D"; }
	const char* GetAesKey() const override { return "94lDsgh4"; }
	// TODO: Response has 7 parameters to implement
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
