#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class BannerClickRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "a5k36D28"; }
	const char* GetAesKey() const override { return "a63Ghbi2"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
