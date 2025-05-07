#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class GatchaActionHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "F7JvPk5H"; }
	const char* GetAesKey() const override { return "bL9fipzaSy7xN2w1"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
