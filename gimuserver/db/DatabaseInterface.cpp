#include "DatabaseInterface.h"

#include <drogon/drogon.h>

#include <algorithm>
#include <stdexcept>

namespace db
{

std::string DatabaseInterface::buildWhere(
	const Cells& lookup,
	size_t from,
	Cells& binds)
{
	std::string sql;
	size_t placeholder = from;

	for (const auto& cell : lookup)
	{
		if (!sql.empty())
		{
			sql += " AND ";
		}

		if (cell.use != Cell::Use::LookupIn)
		{
			sql += cell.name + " = $" + std::to_string(placeholder++);
			binds.push_back(cell);
			continue;
		}

		// An empty IN list would either match nothing or, worse, invite the
		// caller to splice the list into SQL themselves.  Reject it instead.
		if (cell.list.empty())
		{
			LOG_ERROR << "Empty IN list for column: " << cell.name;
			throw std::invalid_argument("Empty IN list for database lookup");
		}

		sql += cell.name + " IN (";
		for (size_t index = 0; index < cell.list.size(); ++index)
		{
			sql += (index == 0 ? "" : ", ") + std::string("$") +
				std::to_string(placeholder++);
			binds.push_back(Lookup(cell.name, cell.list[index]));
		}
		sql += ")";
	}

	return sql;
}

drogon::Task<InterfaceResult<Result>> DatabaseInterface::read(
	const Database database,
	const std::string table,
	const Cells cells)
{
	const auto data = getCellsFor<Use::Data>(cells);
	const auto lookup = getLookupCells(cells);
	validate(database, table, data);

	const auto selectSql = "SELECT " + joinSql(data, [](const Cell& cell, const size_t index) {
			return std::string(index == 0 ? "" : ", ") + cell.name;
		});
	Cells binds;
	const auto whereSql = buildWhere(lookup, 1, binds);

	// Empty lookup means the caller intentionally requested a table-wide read.
	const auto sql = selectSql +
		" FROM " + table +
		(lookup.empty() ? "" : " WHERE " + whereSql) + ";";

	auto binder = *database << sql;
	bind(binder, binds);

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
	const auto lookup = getLookupCells(cells);
	validate(database, table, data);

	const auto setSql = joinSql(data, [](const Cell& cell, const size_t index) {
			return std::string(index == 0 ? "" : ", ") +
				cell.name + " = $" + std::to_string(index + 1);
		});
	Cells binds;
	const auto whereSql = buildWhere(lookup, data.size() + 1, binds);

	// Empty lookup means the caller intentionally requested a table-wide update.
	const auto sql = "UPDATE " + table +
		" SET " + setSql +
		(lookup.empty() ? "" : " WHERE " + whereSql) + ";";

	auto binder = *database << sql;
	bind(binder, data);
	bind(binder, binds);

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

drogon::Task<InterfaceResult<>> DatabaseInterface::upsert(
	const Database database,
	const std::string table,
	const Cells cells,
	const Keys conflict,
	const Keys accumulate)
{
	const auto data = getCellsFor<Use::Data>(cells);
	validate(database, table, data);

	if (conflict.empty())
	{
		LOG_ERROR << "Upsert without a conflict target on table: " << table;
		throw std::invalid_argument("Upsert requires a conflict target");
	}

	Keys keys;
	keys.reserve(data.size());
	for (const auto& cell : data)
	{
		keys.push_back(cell.name);
	}

	const auto isIn = [](const Keys& list, const Key& key) {
		return std::find(list.begin(), list.end(), key) != list.end();
	};

	// Conflict-target columns identify the row, so they are never reassigned.
	std::string setSql;
	for (const auto& key : keys)
	{
		if (isIn(conflict, key))
		{
			continue;
		}

		if (!setSql.empty())
		{
			setSql += ", ";
		}

		setSql += isIn(accumulate, key)
			? key + " = " + key + " + excluded." + key
			: key + " = excluded." + key;
	}

	const auto columnSql = joinSql(keys, [](const Key& key, const size_t index) {
		return std::string(index == 0 ? "" : ", ") + key;
	});
	const auto valueSql = joinSql(data, [](const auto&, const size_t index) {
		return std::string(index == 0 ? "" : ", ") + "$" + std::to_string(index + 1);
	});
	const auto conflictSql = joinSql(conflict, [](const Key& key, const size_t index) {
		return std::string(index == 0 ? "" : ", ") + key;
	});

	// Every data column being part of the conflict target leaves nothing to
	// assign; DO NOTHING is the correct degenerate form.
	const auto sql = "INSERT INTO " + table +
		" (" + columnSql + ") VALUES (" + valueSql + ")" +
		" ON CONFLICT(" + conflictSql + ") DO " +
		(setSql.empty() ? "NOTHING" : "UPDATE SET " + setSql) + ";";

	auto binder = *database << sql;
	bind(binder, data);

	auto result = co_await drogon::orm::internal::SqlAwaiter(std::move(binder));
	co_return InterfaceResult<>{
		.data = {},
		.affected = result.affectedRows(),
	};
}

drogon::Task<InterfaceResult<>> DatabaseInterface::remove(
	const Database database,
	const std::string table,
	const Cells cells)
{
	const auto lookup = getLookupCells(cells);
	validate(database, table, lookup);

	Cells binds;
	const auto whereSql = buildWhere(lookup, 1, binds);

	const auto sql = "DELETE FROM " + table +
		" WHERE " + whereSql + ";";

	auto binder = *database << sql;
	bind(binder, binds);

	auto result = co_await drogon::orm::internal::SqlAwaiter(std::move(binder));
	co_return InterfaceResult<>{
		.data = {},
		.affected = result.affectedRows(),
	};
}

} // namespace db
