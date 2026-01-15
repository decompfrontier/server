#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class ColosseumBattleEndRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "fGDPoQI3"; }
	const char* GetAesKey() const override { return "4V6oIoRC"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
