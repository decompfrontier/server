#include "DeckEditHandler.hpp"
#include "core/System.hpp"
#include "gme/response/SignalKey.hpp"
#include "gme/response/NoticeInfo.hpp"

void Handler::DeckEditHandler::Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const
{
	Json::Value res;

	{
		Response::SignalKey v;
		v.key = "fZnLr4t9";
		v.Serialize(res);
	}

	user.info.Serialize(res);
	user.teamInfo.Serialize(res);

	{
		Response::NoticeInfo v;
		v.url = System::Instance().ServerConfig().NoticeUrl;
		v.Serialize(res);
	}
	
	cb(newGmeOkResponse(GetGroupId(), GetAesKey(), res));
}
