#include "UpdateInfoLightHandler.hpp"

void Handler::UpdateInfoLightHandler::Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const
{
	Json::Value res;
	//cb(newGmeOkResponse(GetGroupId(), GetAesKey(), res));
	cb(newGmeErrorResponse(GetGroupId(), ErrorID::Yes, ErrorOperation::ReturnToGame, "TODO: once I figure out"));
}
