#include "App.hpp"
#include "Handlers.hpp"

// UnitSelectorGachaTicket — handler stub for the "use a Unit Selector Gacha
// Ticket" client request.  Triggered by the user tapping a per-door ticket
// button on a selector gacha (e.g. the Burst Heroes Selector banner under
// the Veteran summon category).
//
// GroupId  = "k57TdKDj"
// AES key  = "1IJ8SaNk"
// (per BraveFrontier IDA readParam and Handler exports.../UnitSelectorGachaTicketRequestHandler.hpp)
//
// Request shape (from bfdata/createbody/UnitSelectorGachaTicketRequest.txt):
//   "CGHaOZda": [{
//     "7Ffmi96v": "<gacha_id>",      target gacha door
//     "XIvaD6Jp": "<selector_id>",   which selector entry within that gacha
//     "pn16CNah": "<unit_id>",       the unit the user picked from the pool
//     "H6k1LIxC": "<?>",             unknown — possibly count or ticket-id
//     "C5QbG2DM": "<?>",             unknown — possibly cost
//   }]
//
// Response: empty `{}` for now.  The full flow needs:
//   1. Verify the user owns at least one matching V2 ticket
//   2. INSERT the selected unit into user_units
//   3. Decrement user_summon_tickets_v2 for the ticket type
//   4. Return UserUnitInfo + UnitSelectorGachaUserInfoResponse (CGHaOZda)
//
// Even an empty `{}` is useful as a registration test — the BF client may
// hide UI elements whose target endpoint isn't registered on the server
// (since tapping would error out).  See handbook §7.11.6 for the broader
// "use ticket" button-visibility investigation.
HANDLEF(UnitSelectorGachaTicket)
{
    (void)session;
    LOG_INFO << "UnitSelectorGachaTicket (stub): " << json;
    co_return HandleResult::success("{}");
}
