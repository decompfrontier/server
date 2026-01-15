#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class BuyCoinStartRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "kJs6CQq1"; }
	const char* GetAesKey() const override { return "J9P7Bmxa"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
