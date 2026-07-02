#include "App.hpp"
#include "Handlers.hpp"

// EventTokenInfo (f49als4D) — sent when the client enters the Event Bazaar
// area inside the Town scene.  The response has 7 fields (Slkc395l, s35idar9,
// qp37xTDh, lDk4fc81, fE2d6ivS, 0Dk4fc81, pn16CNah) whose semantics are
// unknown; returning an empty object keeps the session alive and prevents the
// "Unsupported request" close that previously caused in-flight town upgrades
// to be lost.  Flesh this out once a real capture is available.
HANDLEF(EventTokenInfo)
{
    LOG_INFO << "EventTokenInfo: " << json;
    co_return HandleResult::success("{}");
}
