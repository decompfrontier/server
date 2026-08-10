#include "App.hpp"
#include "Handlers.hpp"

// AreaInfo (Zds63G5y) — fired when the player taps the "Quest" button on the
// home screen.  The client renders the world-map / area-select screen from its
// own local MST data; the server response only needs to not close the session.
// IDA-exported handler stub returns {} (no server-side area state needed yet).
HANDLEF(AreaInfo)
{
    LOG_INFO << "AreaInfo: " << json;
    co_return HandleResult::success("{}");
}
