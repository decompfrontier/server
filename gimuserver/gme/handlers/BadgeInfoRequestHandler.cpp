#include "BadgeInfoRequestHandler.hpp"
#include "gme/response/SignalKey.hpp"
#include "gme/response/UserUnitInfo.hpp"
#include "core/System.hpp"
#include "gme/response/BadgeInfo.hpp"


void Handler::BadgeInfoRequestHandler::Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const
{
	Json::Value res;
	Response::BadgeInfo info;		//How we parse in responses
	info.Serialize(res);			//and include it in the response
	cb(newGmeOkResponse(GetGroupId(), GetAesKey(), res));
}
