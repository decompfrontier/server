#pragma once

/*!
* Defines how an handler function is constructer.
* @param session Drogon session handler
* @param json Decrypted input JSON
* @return Output decrypted JSON
*/
using HandlerFunc = std::function<drogon::Task<std::string>(drogon::SessionPtr session, std::string json)>;

/*!
* Defines the prototype of an handler.
* @param name Name of the handler
*/
#define HANDLE(name) drogon::Task<std::string> name(drogon::SessionPtr session, std::string json)

/*!
* Defines the prototype of an handler with namespace.
* @param name Name of the handler
*/
#define HANDLEF(name) HANDLE(GmeHandlers::name)

// list all available handlers
namespace GmeHandlers
{
	HANDLE(Initialize);
	HANDLE(BadgeInfo);
	HANDLE(ControlCenterEnter);
	HANDLE(DeckEdit);
	HANDLE(FriendGet);
	HANDLE(GatchaList);
	HANDLE(HomeInfo);
	HANDLE(MissionStart);
	HANDLE(UpdateInfoLight);
	HANDLE(UserInfo);
}
