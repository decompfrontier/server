#include "App.hpp"
#include "Handlers.hpp"

#include <gimuserver/db/PacketInterface.hpp>
#include <gimuserver/gme/common/Common.hpp>

HANDLEF(UserInfo)
{
	UserInfoReq req = {};
	const auto& ec = glz::read_json(req, json);
	if (ec)
	{
		const auto& fmte = glz::format_error(ec, json);
		LOG_DEBUG << "Gme UserInfo Error during JSON read: " << fmte;
		co_return HandleResult::error("Deserialization error", fmte);
	}

	// Copy the cached response and build on top of it.
	UserInfoResp resp = theServer()->cache().userInfoResp();

    const auto db = theDb();
	const auto identity = (co_await gme::getUserIdentity(db, req.login_info, true)).nonEmpty();

	// We must have a valid user entry in the database at this point.
	resp.login_info = std::move((co_await gme::getLoginInfo(db, identity)).nonEmpty());
	resp.team_info = std::move((co_await gme::getTeamInfo(db, identity)).nonEmpty());

	// UserInfo is the session-level refresh point for owned units. Clear any
	// persisted new flags before returning the full collection.
	//
	// The client keeps its own in-memory new-unit list and clears it during normal
	// gameplay, so the server only needs to reset the stored flags when the client
	// asks for a fresh UserInfo snapshot.
	co_await db::DatabaseInterface::update(
		db,
		"user_units",
		{
			db::Data("new", false),
			db::Lookup("user_id", identity.userId),
		});

    // We should always have at least one unit, since the game will not let users
    // delete their only unit on the squad.
	resp.unit_info = std::move((co_await db::PacketInterfaceFor<UserUnitInfo>::read(
		db,
		"user_units",
		{ db::Lookup("user_id", identity.userId) })).nonEmpty());

	auto partyDeckInfo = (co_await db::PacketInterfaceFor<UserPartyDeckInfo>::read(
		db,
		"user_decks",
		{ db::Lookup("user_id", identity.userId) })).nonEmpty();
	resp.party_deck_info = std::move(partyDeckInfo);

	resp.unit_dictionary = std::move((co_await db::PacketInterfaceFor<UserUnitDictionary>::read(
		db,
		"user_unit_dictionary",
		{ db::Lookup("user_id", identity.userId) })).data);

    std::string buffer{};
    const auto& ec2 = glz::write_json(resp, buffer);
    if (ec2)
    {
        const auto& glze = glz::format_error(ec2, buffer);
        LOG_DEBUG << "Gme UserInfo Error during JSON writing: " << glze;
        co_return HandleResult::error("Serialization error", glze);
    }

    // Inject PermitPlace unlock data.  The generated PermitPlace struct is a
    // stub ("INVALID" key), so we replace the serialised empty array in-place.
    // Each entry unlocks one entity type — the client reads exactly one key per
    // entry to determine which area/land/gate/mission/dungeon is accessible.
    //   VjCY7rX4 = area, 9C64Qwe0 = land, 0Cq2AlXW = gate,
    //   j28VNcUW = mission, MHx05sXt = dungeon
    //
    // Category roles (empirically established — see handbook §6.9):
    //   - Areas are the MISSION-PARENT TOPOLOGY LINK.  Narrow → no missions
    //     resolve under any land → world map renders empty → click crashes.
    //     Keep at the full 1-1000 range.
    //   - Lands are the CUTSCENE GATE.  Each visible land plays its
    //     `mapN-open.txt` intro cutscene the first time the player enters
    //     Grand Gaia.  Keeping lands at 1-2 caps the cascade at one
    //     cutscene (Mistral intro).
    //   - Gates / missions / dungeons are availability flags only.  Safe
    //     to leave full.
    //
    // TODO: when Cordelica's click-crash is solved, widen lands incrementally
    // (1-3, 1-5, etc.) to expose more chapters.  Each additional land adds
    // one intro cutscene on first session entry.
    //
    // Static-category ranges (from version_info_mst.json):
    //   F_AREA_MST    669 entries, max id ~ 411  → 1-1000  (topology)
    //   F_LAND_MST    147 entries, ~26 unique ids → 1-2    (cutscene gate)
    //   F_GATE_MST     95 entries, ~5  unique ids → 1-100
    //   F_MISSION_MST 1118 entries, 3433 count   → 1-4000
    //   F_DUNGEON_MST 1002 entries, 1532 count   → 1-2000
    // The client silently ignores entries for IDs that don't exist in its local MST.
    static constexpr std::string_view kEmptyPermit = R"("yXNM8kL3":[])";
    static const std::string kFullPermit = []() {
        std::string s;
        s.reserve(200'000);
        s += R"("yXNM8kL3":[)";
        bool first = true;
        auto add = [&](std::string_view key, int id) {
            if (!first) s += ',';
            s += "{\"";
            s += key;
            s += "\":\"";
            s += std::to_string(id);
            s += "\"}";
            first = false;
        };
        for (int i = 1; i <= 1000; ++i) add("VjCY7rX4", i); // areas    (topology)
        for (int i = 1; i <=    2; ++i) add("9C64Qwe0", i); // lands    (cutscene gate)
        for (int i = 1; i <=  100; ++i) add("0Cq2AlXW", i); // gates
        for (int i = 1; i <= 4000; ++i) add("j28VNcUW", i); // missions
        for (int i = 1; i <= 2000; ++i) add("MHx05sXt", i); // dungeons
        s += ']';
        return s;
    }();

    const auto pos = buffer.find(kEmptyPermit);
    if (pos != std::string::npos)
    {
        buffer.replace(pos, kEmptyPermit.size(), kFullPermit);
        LOG_INFO << "UserInfo: PermitPlace injected — areas 1-1000, "
                    "lands 1-2 (cutscene gate), gates 1-100, "
                    "missions 1-4000, dungeons 1-2000 ("
                 << kFullPermit.size() << " bytes)";
    }
    else
        LOG_WARN << "UserInfo: yXNM8kL3 token not found in serialised buffer — PermitPlace not injected";

    co_return HandleResult::success(buffer);
}
