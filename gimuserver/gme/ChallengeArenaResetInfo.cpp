#include "App.hpp"
#include "Handlers.hpp"

HANDLEF(ChallengeArenaResetInfo)
{
	(void)session;
	(void)json;
	co_return HandleResult::success("{}");
}
