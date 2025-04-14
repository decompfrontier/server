#include "GatchaListHandler.hpp"
#include "gme/response/SignalKey.hpp"
#include "core/System.hpp"

void Handler::GatchaListHandler::Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const
{
	Json::Value res;

	{
		Response::SignalKey v;
		v.key = "axhp8Sin";
		v.Serialize(res);
	}

	user.teamInfo.Serialize(res);

	System::Instance().MstConfig().CopyGachaInfoTo(res);

	cb(newGmeOkResponse(GetGroupId(), GetAesKey(), res));
}
