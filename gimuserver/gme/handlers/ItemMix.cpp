#include "App.hpp"
#include "Handlers.hpp"

// ItemMix (4P5GELTF) — item synthesis at the town Synthesis facility. The
// request body (ItemMixRequest::createBody, bfdata/createbody/ItemMixRequest.txt)
// is a single node under "JTf2jY5o" carrying ItemMixRequest::itemCsvData —
// an opaque CSV whose split format is UNVERIFIED (no capture yet; per
// handbook §3.4 we do not guess wire semantics).
//
// The legacy fork acked with an empty OK and the client applies the craft
// locally, so an ack keeps the synthesis menu from erroring. Materials are
// NOT consumed and the crafted item is NOT credited server-side until the
// CSV format is decoded — the INFO log below records the exact body, so the
// first real synthesis attempt in the client produces the capture needed to
// finish this handler (deploy/log/http_log_4P5GELTF_*.log).
//
// TODO(capture): decode itemCsvData → consume recipe materials from
// user_items via recipe_mst, credit the crafted item via gme::addUserItem.
//
// GroupId = "4P5GELTF", AES key = "AFqKIJ8Z4mHPB9xg" (legacy ItemMixRequestHandler).
HANDLEF(ItemMix)
{
	(void)session;
	LOG_INFO << "ItemMix (CAPTURE — itemCsvData format unverified): " << json;
	co_return HandleResult::success("{}");
}
