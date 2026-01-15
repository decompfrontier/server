#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class SGChatLogInfoListRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "jVRl14IS"; }
	const char* GetAesKey() const override { return "sxdFrtRW"; }
	// TODO: Response has 5 parameters to implement
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
