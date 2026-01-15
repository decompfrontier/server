#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class FriendApplyRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "WUNi08YL"; }
	const char* GetAesKey() const override { return "gbpdAEZuv8IP40UF"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
