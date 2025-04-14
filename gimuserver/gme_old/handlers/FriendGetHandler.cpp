#include "FriendGetHandler.hpp"
#include "gme/response/ReinforcementInfo.hpp"

void Handler::FriendGetHandler::Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const
{
	Json::Value res;
	Response::ReinforcementInfo info;
	info.Serialize(res);
	cb(newGmeOkResponse(GetGroupId(), GetAesKey(), res));
}
