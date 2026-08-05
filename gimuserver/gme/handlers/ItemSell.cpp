#include "App.hpp"
#include "Handlers.hpp"

#include <gimuserver/gme/common/Common.hpp>

#include <algorithm>

// ItemSell (qDQerU74) — sell warehouse item stacks for zel. Request
// (ItemSellRequest::createBody, bfdata/createbody/ItemSellRequest.txt):
//
//   "M73i1c5U": [{"n6E8iMf3":"<instance_id>","wgV86x1q":"<count>"}, ...]
//
// Zel credit per unit is ItemMst.sell_price (eKtE6k0n, legacy sellPrice).
// Stacks are decremented but the row is kept at 0 so instance ids stay stable
// for the client's local warehouse model; UserInfo filters zero stacks off
// the wire.
//
// Response: empty OK — the legacy fork acked without a body and the client
// applies the zel/count changes from its own state.
//
// GroupId = "qDQerU74", AES key = "73aFNjPu" (legacy ItemSellRequestHandler).
HANDLEF(ItemSell)
{
	(void)session;
	LOG_INFO << "ItemSell: " << json;

	ItemSellReq req = {};
	if (const auto& ec = glz::read<glz::opts{ .error_on_unknown_keys = false }>(req, json); ec)
	{
		const auto& fmte = glz::format_error(ec, json);
		LOG_DEBUG << "Gme ItemSell Error during JSON read: " << fmte;
		co_return HandleResult::error("Deserialization error", fmte);
	}

	const auto identity = (co_await gme::getUserIdentity(theDb(), req.login_info)).nonEmpty();
	const std::string userId = identity.userId;

	const auto& itemMst = theServer()->cache().itemMst();
	const auto sellPrice = [&itemMst](uint32_t itemId) -> int64_t {
		for (const auto& m : itemMst)
		{
			if (m.id == static_cast<int32_t>(itemId))
				return m.sell_price;
		}
		LOG_WARN << "ItemSell: item " << itemId << " not in ItemMst — sell price defaults to 0";
		return 0;
	};

	int64_t zelCredit = 0;
	for (const auto& e : req.entries)
	{
		try
		{
			const auto rows = co_await theDb()->execSqlCoro(
				"SELECT item_id, item_num FROM user_items "
				"WHERE user_id = $1 AND instance_id = $2;",
				userId, e.instance_id);
			if (rows.size() == 0)
			{
				LOG_WARN << "ItemSell: instance " << e.instance_id << " not found";
				continue;
			}

			const uint32_t itemId = rows[0]["item_id"].as<uint32_t>();
			const uint32_t held   = rows[0]["item_num"].as<uint32_t>();
			const uint32_t sold   = std::min<uint32_t>(held, e.item_num);
			if (sold == 0)
				continue;

			co_await theDb()->execSqlCoro(
				"UPDATE user_items SET item_num = item_num - $1 "
				"WHERE user_id = $2 AND instance_id = $3;",
				sold, userId, e.instance_id);

			zelCredit += sellPrice(itemId) * sold;
			LOG_INFO << "ItemSell: sold " << sold << "x item " << itemId
				<< " (instance " << e.instance_id << ")";
		}
		catch (const drogon::orm::DrogonDbException& ex)
		{
			LOG_ERROR << "ItemSell: DB error for instance " << e.instance_id
				<< ": " << ex.base().what();
		}
	}

	if (zelCredit > 0)
	{
		try
		{
			// Same display-overflow cap the debug CLI enforces: the client
			// wraps zel above 99'999'999 back to 0, which breaks cost checks.
			co_await theDb()->execSqlCoro(
				"UPDATE user_info SET zel = MIN(zel + $1, 99999999) WHERE id = $2;",
				zelCredit, userId);
			LOG_INFO << "ItemSell: credited " << zelCredit << " zel";
		}
		catch (const drogon::orm::DrogonDbException& ex)
		{
			LOG_ERROR << "ItemSell: zel credit failed: " << ex.base().what();
		}
	}

	co_return HandleResult::success("{}");
}
