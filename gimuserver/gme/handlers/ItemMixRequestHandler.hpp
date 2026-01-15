#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class ItemMixRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "4P5GELTF"; }
	const char* GetAesKey() const override { return "AFqKIJ8Z4mHPB9xg"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
