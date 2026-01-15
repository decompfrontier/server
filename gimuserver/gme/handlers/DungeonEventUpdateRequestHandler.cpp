#include "DungeonEventUpdateRequestHandler.hpp"
#include "gme/response/SignalKey.hpp"
#include "gme/response/UserUnitInfo.hpp"
#include "core/System.hpp"

void Handler::DungeonEventUpdateRequestHandler::Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const
{
	Json::Value res;
	
	//TODO: SQL modifications /actionSymbol/4iqcLm1s.php

	cb(newGmeOkResponse(GetGroupId(), GetAesKey(), res));
}
