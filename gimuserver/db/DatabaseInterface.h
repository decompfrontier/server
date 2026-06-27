#pragma once

#include "Types.h"

#include <drogon/drogon.h>

#include <string>

namespace db
{

/*!
* SQL adapter for code that already has database column/value cells.
*
* This class keeps SQL construction centralized instead of scattering ad hoc
* query strings across handlers. It provides a small, explicit call pattern with
* basic validation so database issues are easier to diagnose.
*/
class DatabaseInterface final
{
public:
	using Use = Cell::Use;

	DatabaseInterface() = delete;

	/*!
	* Reads rows from a table.
	*
	* Data cells specify the columns to return. Lookup cells specify optional
	* WHERE predicates; when no lookup cells are provided, the read is table-wide.
	*
	* @param database Database client or transaction to use.
	* @param table SQL table name.
	* @param cells Data and lookup cells for this query.
	* @return Result rows and the number of rows returned.
	*/
	static drogon::Task<InterfaceResult<Result>> read(
		const Database database,
		const std::string table,
		const Cells cells);

	/*!
	* Updates rows in a table.
	*
	* Data cells specify SET assignments. Lookup cells specify optional WHERE
	* predicates; when no lookup cells are provided, the update is table-wide.
	*
	* @param database Database client or transaction to use.
	* @param table SQL table name.
	* @param cells Data and lookup cells for this update.
	* @return Number of affected rows.
	*/
	static drogon::Task<InterfaceResult<>> update(
		const Database database,
		const std::string table,
		const Cells cells);

	/*!
	* Inserts a row into a table.
	*
	* Data cells specify the inserted columns and values. Lookup cells are ignored.
	* Existing rows are ignored by SQLite, and successful inserts return the
	* inserted row so callers can read generated columns.
	*
	* @param database Database client or transaction to use.
	* @param table SQL table name.
	* @param cells Data cells for this insert.
	* @return Inserted row, when SQLite inserted one.
	*/
	static drogon::Task<InterfaceResult<Result>> insert(
		const Database database,
		const std::string table,
		const Cells cells);

	/*!
	* Deletes rows from a table.
	*
	* Lookup cells specify the WHERE predicates. Table-wide deletes are not
	* supported by this helper because callers should be explicit about destructive
	* operations.
	*
	* @param database Database client or transaction to use.
	* @param table SQL table name.
	* @param cells Lookup cells for this delete.
	* @return Number of affected rows.
	*/
	static drogon::Task<InterfaceResult<>> remove(
		const Database database,
		const std::string table,
		const Cells cells);

private:
	/*!
	* Joins SQL fragments produced from an item collection.
	*/
	template <typename Items, typename Formatter>
	static std::string joinSql(const Items& items, Formatter formatter)
	{
		std::string sql;
		size_t index = 0;

		for (const auto& item : items)
		{
			sql += formatter(item, index++);
		}

		return sql;
	}

	/*!
	* Checks the common requirements for a database operation.
	*
	* The cells argument is the required operation data, not necessarily every
	* cell passed by the caller. For example, update validates data cells but not
	* lookup cells because table-wide updates are allowed.
	*/
	static void validate(
		const Database& database,
		const std::string& table,
		const Cells& cells)
	{
		if (!database || table.empty() || cells.empty())
		{
			LOG_ERROR << "Invalid database interface call: "
				<< "db=" << static_cast<bool>(database)
				<< ", table='" << table << "'"
				<< ", cells_empty=" << cells.empty();
			throw std::invalid_argument("Invalid database interface call");
		}
	}

	/*!
	* Filters mixed cells down to either lookup predicates or data values.
	*/
	template <Use CellUse>
	static Cells getCellsFor(const Cells& cells)
	{
		Cells output;
		for (const auto& cell : cells)
		{
			if (cell.use == CellUse)
			{
				output.push_back(cell);
			}
		}

		return output;
	}

	/*!
	* Binds cell values into a Drogon SQL binder in the order they appear.
	*/
	template <typename Binder>
	static void bind(Binder& binder, const Cells& cells)
	{
		for (const auto& cell : cells)
		{
			std::visit([&binder](const auto& typedValue) {
				binder << typedValue;
			}, cell.value);
		}
	}
};

} // namespace db
