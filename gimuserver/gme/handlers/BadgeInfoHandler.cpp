#include "BadgeInfoHandler.hpp"
#include "gme/response/BadgeInfo.hpp"

void Handler::BadgeInfoHandler::Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const
{
	Json::Value res;
	Response::BadgeInfo info;
	info.Serialize(res);
	cb(newGmeOkResponse(GetGroupId(), GetAesKey(), res));
}
