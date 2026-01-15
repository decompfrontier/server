#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class RaidSendChatRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "dYIy6Eu8"; }
	const char* GetAesKey() const override { return "x2Fm1LSt"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
