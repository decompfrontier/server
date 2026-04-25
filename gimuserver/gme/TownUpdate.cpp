#include "App.hpp"
#include "Handlers.hpp"

HANDLEF(TownUpdate)
{
    LOG_INFO << "TownUpdate: " << json;
    co_return HandleResult::success("{}");
}
