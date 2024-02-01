#include "GmeController.hpp"
#include "handlers/InitializeHandler.hpp"
#include "handlers/UserInfoHandler.hpp"
#include "handlers/FriendGetHandler.hpp"

#define REGISTER(code, name) InitializeHandler(#code, []() -> Handler::IHandler* { return new Handler::##name##Handler(); })

void GmeController::InitializeHandlers()
{
	REGISTER(MfZyu1q9, Initialize);
	REGISTER(cTZ3W2JG, UserInfo);
	REGISTER(2o4axPIC, FriendGet);
}
