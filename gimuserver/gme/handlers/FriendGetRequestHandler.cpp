#include "FriendGetRequestHandler.hpp"
#include "gme/response/SignalKey.hpp"
#include "gme/response/UserUnitInfo.hpp"
#include "core/System.hpp"
#include "gme/response/ReinforcementInfo.hpp"

void Handler::FriendGetRequestHandler::Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const
{
	Json::Value res;
	Response::ReinforcementInfo info;
	info.Serialize(res);
	cb(newGmeOkResponse(GetGroupId(), GetAesKey(), res));
}
