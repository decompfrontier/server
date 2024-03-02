#include "GmeController.hpp"
#include "handlers/InitializeHandler.hpp"
#include "handlers/UserInfoHandler.hpp"
#include "handlers/FriendGetHandler.hpp"
#include "handlers/HomeInfoHandler.hpp"
#include "handlers/BadgeInfoHandler.hpp"

#define REGISTER(name) InitializeHandler(std::make_shared<Handler::##name##Handler>())

void GmeController::InitializeHandlers()
{
	REGISTER(Initialize);
	REGISTER(UserInfo);
	REGISTER(FriendGet);
	REGISTER(HomeInfo);
	REGISTER(BadgeInfo);
}
