#include "App.hpp"
#include "Handlers.hpp"

// Stub. Old fork's UnitEvoRequestHandler returns an empty response object
// without inspecting the request body. Real implementation needs:
//   - hashed-key UnitMst loaded into ServerCache::unitMst() (see ServerCache.cpp)
//   - the real Req/Resp KDL fields filled in (currently placeholders in unit.kdl)
//   - SELECT base unit, look up evolved form, UPDATE preserving IMP stats,
//     DELETE materials, debit zel.
HANDLEF(UnitEvo)
{
	(void)session;
	(void)json;
	co_return HandleResult::success("{}");
}
