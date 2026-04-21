#include "App.hpp"
#include "Handlers.hpp"

// Toggle the favorite/lock flag on user_units. Old fork only echoed the flag
// back without persisting; this port also writes favorite_flg to user_units
// so the state survives across sessions. Persistence is best-effort: if the
// row does not exist (e.g. unit_id missing in the seeded table), the UPDATE
// silently no-ops and the response still echoes the requested flag.
HANDLEF(UnitFavorite)
{
	UnitFavoriteReq req = {};
	if (const auto& ec = glz::read_json(req, json); ec)
	{
		const auto& fmte = glz::format_error(ec, json);
		LOG_DEBUG << "Gme UnitFavorite Error during JSON read: " << fmte;
		co_return HandleResult::error("Deserialization error", fmte);
	}

	if (req.entries.empty())
	{
		co_return HandleResult::error("UnitFavorite: empty entries");
	}

	// TODO: replace with session-derived user id once auth is wired up.
	// Matches the placeholder used elsewhere in this tree (UserInfo.cpp).
	const std::string userId = "0839899613932562";

	for (const auto& e : req.entries)
	{
		try
		{
			co_await theDb()->execSqlCoro(
				"UPDATE user_units SET favorite_flg=$1 WHERE user_id=$2 AND id=$3",
				e.favorite, userId, e.user_unit_id);
		}
		catch (const drogon::orm::DrogonDbException& ex)
		{
			LOG_WARN << "UnitFavorite: UPDATE failed for unit " << e.user_unit_id << ": " << ex.base().what();
		}
	}

	UnitFavoriteResp resp = {};
	resp.entry = req.entries.front();

	std::string buffer{};
	if (const auto& ec2 = glz::write_json(resp, buffer); ec2)
	{
		const auto& glze = glz::format_error(ec2, buffer);
		LOG_DEBUG << "Gme UnitFavorite Error during JSON writing: " << glze;
		co_return HandleResult::error("Serialization error", glze);
	}

	co_return HandleResult::success(buffer);
}
