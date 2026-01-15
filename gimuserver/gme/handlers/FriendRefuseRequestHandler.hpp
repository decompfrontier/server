#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class FriendRefuseRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "N5SQd7oA"; }
	const char* GetAesKey() const override { return "fk8YaVy9puJbnLWv"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
