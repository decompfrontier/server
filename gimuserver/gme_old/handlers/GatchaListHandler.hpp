#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class GatchaListHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "Uo86DcRh"; }
	const char* GetAesKey() const override { return "8JbxFvuSaB2CK7Ln"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
