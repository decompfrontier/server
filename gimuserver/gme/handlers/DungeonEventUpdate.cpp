#include "App.hpp"
#include "Handlers.hpp"

// DungeonEventUpdate (BjAt1D6b / k5EiNe9x) — fires when the client enters
// the Grand Gaia world map after the opening cutscene.  The original handler
// returns an empty JSON object; no fields are read by the client from this
// response.  Without this handler the server returns "Unsupported request"
// and the client hard-crashes out of the cutscene.
HANDLEF(DungeonEventUpdate)
{
    LOG_INFO << "DungeonEventUpdate: " << json;
    co_return HandleResult::success("{}");
}

// GetScenarioPlayingInfo now lives in Scenario.cpp — it returns the real
// viewed-cutscene set from user_scenarios instead of the old {} stub.

// UpdatePermitPlaceInfo (1MJT6L3W / 3zip5Htw) — sent after entering an
// area to refresh the server-side permit-place allow-list.  Original handler
// returns {}.  Our PermitPlace is injected once in UserInfo so no update
// action is required here.
HANDLEF(UpdatePermitPlaceInfo)
{
    LOG_INFO << "UpdatePermitPlaceInfo: " << json;
    co_return HandleResult::success("{}");
}

// UpdateEventInfo (rCB7ZI8x / L1o4eGbi) — updates the client's view of
// active event data.  Original handler returns {}.
HANDLEF(UpdateEventInfo)
{
    LOG_INFO << "UpdateEventInfo: " << json;
    co_return HandleResult::success("{}");
}

// Chronology (5o8ZlDGX / SNrhAG29) — Chronology timeline feature endpoint.
// Original handler returns {}.
HANDLEF(Chronology)
{
    LOG_INFO << "Chronology: " << json;
    co_return HandleResult::success("{}");
}
