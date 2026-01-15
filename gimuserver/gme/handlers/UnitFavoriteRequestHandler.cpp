#include "UnitFavoriteRequestHandler.hpp"
#include "gme/response/SignalKey.hpp"
#include "gme/response/UserUnitInfo.hpp"
#include "core/System.hpp"

void Handler::UnitFavoriteRequestHandler::Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const
{
	Json::Value res;
	
	//std::cout << req.toStyledString();

	Json::Value d;
	d["edy7fq3L"] = req["3kcmQy7B"][0]["edy7fq3L"]; // Unique String for Unit
	d["5JbjC3Pp"] = req["3kcmQy7B"][0]["5JbjC3Pp"]; //Favorited or not?
	res["3kcmQy7B"] = d;

	/*
	*
	* if (req.isMember("user_id")) {
		requestUserId = req["user_id"].asString();
		user.info.userID = requestUserId;
		LOG_INFO << "UserInfoHandler: Overriding user_id from request body to: " << requestUserId;
	}
	*
	*

	"3kcmQy7B" :
	[
		{
			"2pAyFjmZ" : "1",
			"5JbjC3Pp" : "1",
			"edy7fq3L" : "11843"
		}

	*/



	cb(newGmeOkResponse(GetGroupId(), GetAesKey(), res));
}
