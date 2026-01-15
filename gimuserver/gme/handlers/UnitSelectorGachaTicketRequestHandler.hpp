#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class UnitSelectorGachaTicketRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "k57TdKDj"; }
	const char* GetAesKey() const override { return "1IJ8SaNk"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
