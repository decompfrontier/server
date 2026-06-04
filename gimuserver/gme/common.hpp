#pragma once

#include "Handlers.hpp"

#include <drogon/orm/DbClient.h>

#include <cstdint>
#include <exception>
#include <stdexcept>

/*!
* Awaits a Drogon database task in a GME handler. Convenience macro to
* reduce boilerplate and enforce consistent error handling for database operations
* in handlers.
*/
#define CO_AWAIT_DB(expression) \
	try \
	{ \
		(void)co_await (expression); \
	} \
	catch (const drogon::orm::DrogonDbException& ex) \
	{ \
		LOG_ERROR << "Database error: " << ex.base().what(); \
		co_return HandleResult::error("Database error", ex.base().what()); \
	} \
	catch (const std::exception& ex) \
	{ \
		LOG_ERROR << "Database error: " << ex.what(); \
		co_return HandleResult::error("Database error", ex.what()); \
	}

namespace gme
{

using Result = drogon::orm::Result;

/*!
* Awaits a database operation and requires its result to contain or affect at
* least one row.
*/
template <typename Operation>
drogon::Task<Result> nonEmpty(Operation operation)
{
	auto result = co_await operation;
	if (result.empty() && result.affectedRows() == 0)
	{
		throw std::runtime_error("Database query returned or affected no rows");
	}

	co_return result;
}

/*!
* Runs database operations sequentially in one transaction.
*
* Each operation receives the transaction database client and must return a
* Drogon database task. Operations define their own result expectations. The
* transaction is rolled back before any failure is propagated to the caller;
* otherwise, Drogon commits it when the transaction's final owner releases it.
*/
template <typename... Operations>
drogon::Task<void> runTransaction(
	const drogon::orm::DbClientPtr db,
	Operations... operations)
{
	if (!db)
	{
		throw std::invalid_argument("Invalid runTransaction call");
	}

	auto transaction = co_await db->newTransactionCoro();
	try
	{
		(co_await operations(transaction), ...);
	}
	catch (...)
	{
		transaction->rollback();
		throw;
	}
}

/*!
* Derives the tutorial completion flag sent in LoginInfoResp.
*
* The database stores tutorial_status. Before a login-info packet is sent back,
* handlers use this helper to keep tutorial_end_flag consistent with that
* status. In the current server model, status 12 or greater means the tutorial
* is complete.
* @param status The tutorial_status to derive the end flag from.
*/
inline bool getTutorialEndFlag(uint32_t status)
{
	return status >= 12;
}

/*!
* Derives the deck cost for a given user level based on the progression data.
*/
inline int32_t deckCostForLevel(int32_t level)
{
	const auto& progression = theServer()->cache().initializeResp().progression;
	for (const auto& levelMst : progression)
	{
		if (levelMst.level == level)
		{
			return levelMst.deck_cost;
		}
	}

	return 0;
}

}
