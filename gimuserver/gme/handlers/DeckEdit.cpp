#include "App.hpp"
#include "Handlers.hpp"

#include <gimuserver/gme/common/Common.hpp>
HANDLEF(DeckEdit)
{
	DeckEditReq req = {};
	const auto& ec = glz::read<glz::opts{ .error_on_unknown_keys = false }>(req, json);
	if (ec)
	{
		const auto error = glz::format_error(ec, json);
		LOG_ERROR << "DeckEditReq deserialization failed:\n" << error;
		co_return HandleResult::error("Deserialization error", error);
	}

	const auto identity = (co_await gme::getUserIdentity(theDb(), req.login_info, true)).nonEmpty();

	{
		auto transaction = co_await theDb()->newTransactionCoro();
		try
		{
			(co_await db::DatabaseInterface::update(
				transaction,
				"user_info",
				{
					db::Data("active_deck", req.team_info.active_deck),
					db::Lookup("id", identity.userId),
				})).nonEmpty();

			if (!req.party_deck_info.empty())
			{
				(co_await gme::updateDecks(
					transaction,
					identity,
					req.party_deck_info)).nonEmpty();
			}
		}
		catch (...)
		{
			transaction->rollback();
			throw;
		}
	}

	co_return HandleResult::success("{}");
}
