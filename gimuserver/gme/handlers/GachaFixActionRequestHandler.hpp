#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class GachaFixActionRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "MFLFP9f2"; }
	const char* GetAesKey() const override { return "484bnJG2"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
