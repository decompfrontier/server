#include "App.hpp"
#include "Handlers.hpp"

// FixGiftInfo (gLRIn74v) — fired immediately after MissionEnd to reconcile
// the gift/reward inbox.  Old-tree implementation returned {} unconditionally.
HANDLEF(FixGiftInfo)
{
    LOG_INFO << "FixGiftInfo: " << json;
    co_return HandleResult::success("{}");
}
