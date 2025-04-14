#include "GmeController.hpp"
#include "handlers/InitializeHandler.hpp"
#include "handlers/UserInfoHandler.hpp"
#include "handlers/FriendGetHandler.hpp"
#include "handlers/HomeInfoHandler.hpp"
#include "handlers/BadgeInfoHandler.hpp"
#include "handlers/ControlCenterEnterHandler.hpp"
#include "handlers/UpdateInfoLightHandler.hpp"
#include "handlers/GatchaListHandler.hpp"
#include "handlers/DeckEditHandler.hpp"
#include "handlers/MissionStartHandler.hpp"

#define REGISTER(name) InitializeHandler(std::make_shared<Handler::##name##Handler>())

void GmeController::InitializeHandlers()
{
	REGISTER(Initialize);
	REGISTER(UserInfo);
	REGISTER(FriendGet);
	REGISTER(HomeInfo);
	REGISTER(BadgeInfo);
	REGISTER(ControlCenterEnter);
	REGISTER(UpdateInfoLight);
	REGISTER(GatchaList);
	REGISTER(DeckEdit);
	REGISTER(MissionStart);
}
