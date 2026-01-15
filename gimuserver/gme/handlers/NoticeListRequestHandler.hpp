#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class NoticeListRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "5s4aVWfc"; }
	const char* GetAesKey() const override { return "miMBpUZ3"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
