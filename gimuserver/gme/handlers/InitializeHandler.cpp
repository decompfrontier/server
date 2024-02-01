#include "InitializeHandler.hpp"
#include "core/Utils.hpp"
#include "core/System.hpp"
#include "gme/response/UserInfo.hpp"
#include "gme/response/SignalKey.hpp"

bool Handler::InitializeHandler::Handle(const Json::Value& req, Json::Value& res)
{
	Response::UserInfo info;

	// TODO: add drogon sqlite framework here...
	info.userID = "00000000";
	info.handleName = "Arves100";
	info.friendID = "000000001";
	info.Serialize(res);

	Response::SignalKey key;
	key.key = "AAAAAAAA";
	key.Serialize(res);

	System::Instance().MstInfo().GetMstData(res);

	return true;
}

