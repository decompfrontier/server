#include "App.hpp"
#include "Handlers.hpp"

// ItemEdit (ruoB7bD8) — fired during mission prep when the player sets the
// battle-item loadout (potions/spheres carried into the fight), alongside the
// party/reinforcement select. The request body (ItemEditRequest::createBody,
// bfdata/createbody/ItemEditRequest.txt) is UserEquipItemInfo + bonus-item
// lists; the client tracks the loadout itself and MissionEnd reports what was
// consumed, so no response body is required.
//
// The legacy server (BF-WorkingDir ItemEditRequestHandler) simply acked with an
// empty OK — matched here. Without this handler the dispatcher returns
// "Unsupported request: ruoB7bD8", which the client renders as a Close dialog
// back to home on quest entry.
//
// GroupId = "ruoB7bD8", AES key = "DHEfRexCu0q5TAQm" (from the legacy
// ItemEditRequestHandler GetGroupId/GetAesKey).
//
// TODO (optional): persist the selected loadout to user_items equip state so it
// survives a reload; the client re-sends it each mission, so an ack suffices for
// now.
HANDLEF(ItemEdit)
{
    (void)session;
    LOG_INFO << "ItemEdit: " << json;
    co_return HandleResult::success("{}");
}
