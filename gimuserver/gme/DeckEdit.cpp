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

    // The client sends FULL multi-deck state in one DeckEdit packet — captured
    // requests contain entries spanning deck_num 0..9 inclusive (one row per
    // member per deck).  Earlier the handler DELETEd only req.deck[0]'s slot
    // and then INSERTed every entry, which left the rows for decks 1..9
    // colliding with the prior save's PRIMARY KEY (user_id, deck_type,
    // deck_num, disp_order).  The leader row at disp_order=0 always conflicts,
    // so newly-edited teams kept their old leader (the default unit 10001 from
    // first-launch seeding), making "take team 4 into battle" load what
    // appeared to be team 1's leader.
    //
    // Fix: collect every (deck_type, deck_num) slot referenced in the request
    // and DELETE each slot before INSERTing the new rows.  Works for both
    // full-state saves (10 slots) and any future partial save the client
    // might send.

    std::vector<std::pair<int32_t, int32_t>> slotsToClear;
    slotsToClear.reserve(req.deck.size());
    for (const auto& e : req.deck)
    {
        const auto slot = std::make_pair(e.deck_type, e.deck_num);
        bool seen = false;
        for (const auto& s : slotsToClear) { if (s == slot) { seen = true; break; } }
        if (!seen) slotsToClear.emplace_back(slot);
    }

    LOG_INFO << "DeckEdit: saving " << req.deck.size() << " members across "
             << slotsToClear.size() << " deck slot(s)";

    for (const auto& [dt, dn] : slotsToClear)
    {
        try
        {
            co_await theDb()->execSqlCoro(
                "DELETE FROM user_party_decks"
                " WHERE user_id=$1 AND deck_type=$2 AND deck_num=$3;",
                std::string(kUserId), dt, dn);
        }
        catch (const drogon::orm::DrogonDbException& ex)
        {
            LOG_WARN << "DeckEdit: DELETE failed for deck_type=" << dt
                     << " deck_num=" << dn << ": " << ex.base().what();
            // Non-fatal — proceed to insert.
        }
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
            LOG_WARN << "DeckEdit: INSERT failed for deck_num=" << e.deck_num
                     << " disp_order=" << e.disp_order
                     << ": " << ex.base().what();
        }
    }

    // Persist the active deck selection.  The captured DeckEdit request carries
    // an extra group "Ti62XfZK" whose first element holds Z0Y4RoD7 — the client
    // is telling us "this is now the active deck" at the moment of save.  We
    // extract it with a substring scan rather than declaring another glaze
    // struct because the inner item also contains unrelated state (arena
    // counters etc.) we don't otherwise track, and a strict struct would risk
    // the same kind of silent parse failure that previously broke MissionStart.
    //
    // Without this, the home scene's "current team" would stay on deck 0 even
    // after the player edited and selected team 2/3/4 — they'd have to start a
    // mission first for MissionStart's UPDATE to fire.
    {
        const std::string_view ti = "\"Ti62XfZK\":[";
        const auto tiPos = json.find(ti);
        if (tiPos != std::string::npos)
        {
            const std::string_view needle = "\"Z0Y4RoD7\":\"";
            const auto pos = json.find(needle, tiPos);
            if (pos != std::string::npos)
            {
                const auto valStart = pos + needle.size();
                const auto valEnd   = json.find('"', valStart);
                if (valEnd != std::string::npos && valEnd > valStart)
                {
                    try
                    {
                        const int32_t newActiveDeck = std::stoi(
                            std::string(json.substr(valStart, valEnd - valStart)));
                        co_await theDb()->execSqlCoro(
                            "UPDATE userinfo SET active_deck=$2 WHERE id=$1;",
                            std::string(kUserId), newActiveDeck);
                        LOG_INFO << "DeckEdit: active_deck set to " << newActiveDeck;
                    }
                    catch (const std::exception& ex)
                    {
                        LOG_WARN << "DeckEdit: active_deck UPDATE failed: " << ex.what();
                    }
                }
            }
        }
    }

    // Echo the saved deck back under "dX7S2Lc1".
    DeckEditResp resp;
    resp.deck = req.deck;
    co_return HandleResult::success(glz::write_json(resp).value_or("{}"));
}
