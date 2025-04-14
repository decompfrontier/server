#include "MissionStartHandler.hpp"

void Handler::MissionStartHandler::Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const
{
	Json::Value res;

	cb(newGmeOkResponse(GetGroupId(), GetAesKey(), res));
}
