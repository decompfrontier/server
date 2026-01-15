#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class UpdatePermitPlaceInfoRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "1MJT6L3W"; }
	const char* GetAesKey() const override { return "3zip5Htw"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
