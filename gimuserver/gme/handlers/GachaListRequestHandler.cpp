#include "GachaListRequestHandler.hpp"
#include "gme/response/SignalKey.hpp"
#include "gme/response/UserUnitInfo.hpp"
#include "core/System.hpp"

void Handler::GachaListRequestHandler::Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const
{
	Json::Value res;

	//TODO: Tuck into a response class

	{
		Response::SignalKey v;
		v.key = "axhp8Sin";
		v.Serialize(res);
	}

	user.teamInfo.Serialize(res);

	System::Instance().MstConfig().CopyGachaInfoTo(res);
	
	cb(newGmeOkResponse(GetGroupId(), GetAesKey(), res));
}
