#include "App.hpp"
#include "Handlers.hpp"

// DeckEdit (m2Ve9PkJ) — fired when the player saves a party deck in the
// squad-management screen.  The client sends the full desired composition for
// one deck slot under "dX7S2Lc1"; we DELETE the existing entries for that
// (deck_type, deck_num) pair and INSERT the new ones, then echo the updated
// list back under the same key so the client can render it.
//
// Request / response key map (from glz::meta<UserPartyDeckInfo>):
//   "dX7S2Lc1" — array of deck-member entries
//   "zsiAn9P1" — deck_num (plain int)
//   "U9ABSYEp" — deck_type (plain int, usually 1)
//   "gr48vsdJ" — member_type (quoted int: 0=leader, 1=member, 2=sub)
//   "edy7fq3L" — user_unit_id (quoted int)
//   "XuJL4pc5" — disp_order (quoted int)

struct DeckEditReq {
    std::vector<UserPartyDeckInfo> deck;
};
template <> struct glz::meta<DeckEditReq> {
    using T = DeckEditReq;
    static constexpr auto value = glz::object("dX7S2Lc1", &T::deck);
};

struct DeckEditResp {
    std::vector<UserPartyDeckInfo> deck;
};
template <> struct glz::meta<DeckEditResp> {
    using T = DeckEditResp;
    static constexpr auto value = glz::object("dX7S2Lc1", &T::deck);
};

HANDLEF(DeckEdit)
{
    LOG_INFO << "DeckEdit: " << json;

    static constexpr std::string_view kUserId = "0839899613932562";

    DeckEditReq req{};
    if (const auto ec = glz::read<glz::opts{.error_on_unknown_keys = false}>(req, json); ec)
    {
        LOG_WARN << "DeckEdit: parse error: " << glz::format_error(ec, json);
        co_return HandleResult::success("{}");
    }

    if (req.deck.empty())
    {
        LOG_WARN << "DeckEdit: empty deck array — nothing to save";
        co_return HandleResult::success("{}");
    }

    const int32_t deckType = req.deck[0].deck_type;
    const int32_t deckNum  = req.deck[0].deck_num;

    LOG_INFO << "DeckEdit: saving deck_type=" << deckType
             << " deck_num=" << deckNum
             << " (" << req.deck.size() << " members)";

    // Remove existing entries for this deck slot, then insert the new ones.
    try
    {
        co_await theDb()->execSqlCoro(
            "DELETE FROM user_party_decks"
            " WHERE user_id=$1 AND deck_type=$2 AND deck_num=$3;",
            std::string(kUserId), deckType, deckNum);
    }
    catch (const drogon::orm::DrogonDbException& ex)
    {
        LOG_WARN << "DeckEdit: DELETE failed: " << ex.base().what();
        // Non-fatal — proceed to insert.
    }

    for (const auto& e : req.deck)
    {
        try
        {
            co_await theDb()->execSqlCoro(
                "INSERT INTO user_party_decks"
                " (user_id, deck_type, deck_num, user_unit_id, member_type, disp_order)"
                " VALUES ($1,$2,$3,$4,$5,$6);",
                std::string(kUserId),
                e.deck_type,
                e.deck_num,
                e.user_unit_id,
                e.member_type,
                e.disp_order);
        }
        catch (const drogon::orm::DrogonDbException& ex)
        {
            LOG_WARN << "DeckEdit: INSERT failed for disp_order="
                     << e.disp_order << ": " << ex.base().what();
        }
    }

    // Echo the saved deck back under "dX7S2Lc1".
    DeckEditResp resp;
    resp.deck = req.deck;
    co_return HandleResult::success(glz::write_json(resp).value_or("{}"));
}
