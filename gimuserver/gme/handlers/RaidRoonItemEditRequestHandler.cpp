#include "RaidRoonItemEditRequestHandler.hpp"
#include "gme/response/SignalKey.hpp"
#include "gme/response/UserUnitInfo.hpp"
#include "core/System.hpp"

void Handler::RaidRoonItemEditRequestHandler::Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const
{
	Json::Value res;
	
	cb(newGmeOkResponse(GetGroupId(), GetAesKey(), res));
}
