#include "App.hpp"
#include "Handlers.hpp"

HANDLEF(UserInfo)
{
	co_return HandleResult::error("Unsupported");

}
