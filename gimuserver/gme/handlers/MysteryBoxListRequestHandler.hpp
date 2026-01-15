#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class MysteryBoxListRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "pAJ2Xesw"; }
	const char* GetAesKey() const override { return "DaswA3rE"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
