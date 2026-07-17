#include "App.hpp"
#include "Handlers.hpp"

#include <gimuserver/gme/common/Common.hpp>

// ItemFavorite (I8il6EiI) — favorite/lock warehouse item stacks. The client
// sends the COMPLETE current favorite set on every call (only flag=1 entries;
// ItemFavoriteRequest::createBody skips rows whose getFavoFlg() != 1), so the
// server resets favorite_flg for the user and re-marks the listed instance
// ids. An empty list is therefore valid — it means "nothing is favorited".
//
//   Request : {"VSRPkdId":[{"n6E8iMf3":"<instance_id>","5JbjC3Pp":"1"},...]}
//   Response: same shape echoed back (legacy ItemFavoriteResponse serialised
//             an array of instance ids under VSRPkdId; the client readParam
//             reads n6E8iMf3 + 5JbjC3Pp per node).
//
// The legacy fork acked without persisting; this port writes user_items.
// favorite_flg so locks survive a reload (UserInfo reports them back through
// UserInfoResp.item_favorite).
//
// GroupId = "I8il6EiI", AES key = "aRoIftRy" (legacy ItemFavoriteRequestHandler).
HANDLEF(ItemFavorite)
{
	(void)session;
	LOG_INFO << "ItemFavorite: " << json;

	ItemFavoriteReq req = {};
	if (const auto& ec = glz::read<glz::opts{ .error_on_unknown_keys = false }>(req, json); ec)
	{
		const auto& fmte = glz::format_error(ec, json);
		LOG_DEBUG << "Gme ItemFavorite Error during JSON read: " << fmte;
		co_return HandleResult::error("Deserialization error", fmte);
	}

	const std::string userId = co_await gme::getSoleUserId(theDb());
	if (userId.empty())
	{
		co_return HandleResult::error("ItemFavorite: no user");
	}

	try
	{
		// The request is the full favorite set: clear-then-set.
		co_await theDb()->execSqlCoro(
			"UPDATE user_items SET favorite_flg = 0 WHERE user_id = $1;", userId);

		for (const auto& e : req.entries)
		{
			co_await theDb()->execSqlCoro(
				"UPDATE user_items SET favorite_flg = $1 "
				"WHERE user_id = $2 AND instance_id = $3;",
				e.favorite, userId, e.instance_id);
		}
	}
	catch (const drogon::orm::DrogonDbException& ex)
	{
		LOG_ERROR << "ItemFavorite: DB error: " << ex.base().what();
	}

	ItemFavoriteResp resp = {};
	resp.entries = req.entries;

	std::string buffer{};
	if (const auto& ec2 = glz::write_json(resp, buffer); ec2)
	{
		const auto& glze = glz::format_error(ec2, buffer);
		LOG_DEBUG << "Gme ItemFavorite Error during JSON writing: " << glze;
		co_return HandleResult::error("Serialization error", glze);
	}

	co_return HandleResult::success(buffer);
}
