#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class FrontierGateSaveRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "Ng73nFHJ"; }
	const char* GetAesKey() const override { return "4SdtoczN"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
