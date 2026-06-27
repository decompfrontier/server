#include "DatabaseInterface.h"

#include <drogon/drogon.h>

#include <stdexcept>

namespace db
{

drogon::Task<InterfaceResult<Result>> DatabaseInterface::read(
	const Database database,
	const std::string table,
	const Cells cells)
{
	const auto data = getCellsFor<Use::Data>(cells);
	const auto lookup = getCellsFor<Use::Lookup>(cells);
	validate(database, table, data);

	const auto selectSql = "SELECT " + joinSql(data, [](const Cell& cell, const size_t index) {
			return std::string(index == 0 ? "" : ", ") + cell.name;
		});
	const auto whereSql = joinSql(lookup, [](const Cell& cell, const size_t index) {
		return std::string(index == 0 ? "" : " AND ") +
			cell.name + " = $" + std::to_string(index + 1);
	});

	// Empty lookup means the caller intentionally requested a table-wide read.
	const auto sql = selectSql +
		" FROM " + table +
		(lookup.empty() ? "" : " WHERE " + whereSql) + ";";

	auto binder = *database << sql;
	bind(binder, lookup);

	auto result = co_await drogon::orm::internal::SqlAwaiter(std::move(binder));
	co_return InterfaceResult<Result>{
		.data = result,
		.affected = result.size(),
	};
}

drogon::Task<InterfaceResult<>> DatabaseInterface::update(
	const Database database,
	const std::string table,
	const Cells cells)
{
	const auto data = getCellsFor<Use::Data>(cells);
	const auto lookup = getCellsFor<Use::Lookup>(cells);
	validate(database, table, data);

	const auto setSql = joinSql(data, [](const Cell& cell, const size_t index) {
			return std::string(index == 0 ? "" : ", ") +
				cell.name + " = $" + std::to_string(index + 1);
		});
	const auto whereSql = joinSql(lookup, [from = data.size() + 1](const Cell& cell, const size_t index) {
		return std::string(index == 0 ? "" : " AND ") +
			cell.name + " = $" + std::to_string(from + index);
	});

	// Empty lookup means the caller intentionally requested a table-wide update.
	const auto sql = "UPDATE " + table +
		" SET " + setSql +
		(lookup.empty() ? "" : " WHERE " + whereSql) + ";";

	auto binder = *database << sql;
	bind(binder, data);
	bind(binder, lookup);

	auto result = co_await drogon::orm::internal::SqlAwaiter(std::move(binder));
	co_return InterfaceResult<>{
		.data = {},
		.affected = result.affectedRows(),
	};
}

drogon::Task<InterfaceResult<Result>> DatabaseInterface::insert(
	const Database database,
	const std::string table,
	const Cells cells)
{
	const auto data = getCellsFor<Use::Data>(cells);
	validate(database, table, data);

	Keys keys;
	keys.reserve(data.size());
	for (const auto& cell : data)
	{
		keys.push_back(cell.name);
	}

	// RETURNING lets callers read generated columns such as autoincrement ids.
	const auto sql = "INSERT OR IGNORE INTO " + table +
		" (" + joinSql(keys, [](const Key& key, const size_t index) {
			return std::string(index == 0 ? "" : ", ") + key;
		}) + ") VALUES (" + joinSql(data, [](const auto&, const size_t index) {
			return std::string(index == 0 ? "" : ", ") +
				"$" + std::to_string(index + 1);
		}) + ") RETURNING *;";

	auto binder = *database << sql;
	bind(binder, data);

	auto result = co_await drogon::orm::internal::SqlAwaiter(std::move(binder));
	co_return InterfaceResult<Result>{
		.data = result,
		.affected = result.empty() ? result.affectedRows() : result.size(),
	};
}

drogon::Task<InterfaceResult<>> DatabaseInterface::remove(
	const Database database,
	const std::string table,
	const Cells cells)
{
	const auto lookup = getCellsFor<Use::Lookup>(cells);
	validate(database, table, lookup);

	const auto whereSql = joinSql(lookup, [](const Cell& cell, const size_t index) {
		return std::string(index == 0 ? "" : " AND ") +
			cell.name + " = $" + std::to_string(index + 1);
	});

	const auto sql = "DELETE FROM " + table +
		" WHERE " + whereSql + ";";

	auto binder = *database << sql;
	bind(binder, lookup);

	auto result = co_await drogon::orm::internal::SqlAwaiter(std::move(binder));
	co_return InterfaceResult<>{
		.data = {},
		.affected = result.affectedRows(),
	};
}

} // namespace db
