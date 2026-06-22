#pragma once

#include <drogon/orm/DbClient.h>
#include <drogon/orm/Result.h>

#include <cstdint>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

namespace db
{

using Key = std::string;
using Keys = std::vector<Key>;

using Row = drogon::orm::Row;
using Result = drogon::orm::Result;
using Database = drogon::orm::DbClientPtr;

// Value types currently supported by the database interface binder.
using Value = std::variant<int32_t, int64_t, uint32_t, std::string, bool>;
using Values = std::vector<Value>;

// One named database value plus its role in a SQL operation.
struct Cell
{
	// How this cell should be used when building SQL.
	enum class Use
	{
		Lookup,
		Data,
	};

	Use use;
	Key name;
	Value value;
};
using Cells = std::vector<Cell>;

/*!
* Builds a lookup cell for WHERE predicates.
*/
inline Cell Lookup(Key name, Value value)
{
	return {
		.use = Cell::Use::Lookup,
		.name = std::move(name),
		.value = std::move(value),
	};
}

/*!
* Builds a data cell for SELECT, UPDATE, or INSERT values.
*/
inline Cell Data(Key name, Value value)
{
	return {
		.use = Cell::Use::Data,
		.name = std::move(name),
		.value = std::move(value),
	};
}

/*!
* Result wrapper returned by database interfaces.
*
* The data payload depends on the operation. affected stores the number of rows
* returned or affected so callers can consistently require a non-empty result.
*/
template <typename T = std::monostate>
struct InterfaceResult
{
	T data;
	size_t affected = 0;

	/*!
	* Requires this result to contain or affect at least one row.
	*/
	T& nonEmpty()
	{
		if (affected == 0)
		{
			throw std::runtime_error("Database operation affected no rows");
		}

		return data;
	}

	/*!
	* Requires this result to contain or affect at least one row.
	*/
	const T& nonEmpty() const
	{
		if (affected == 0)
		{
			throw std::runtime_error("Database operation affected no rows");
		}

		return data;
	}

	/*!
	* Reads a typed field from the first database row after requiring a
	* non-empty result.
	*/
	template <typename FieldType, typename U = T>
		requires std::is_same_v<U, Result>
	FieldType front(const Key& key) const
	{
		return nonEmpty().front()[key].template as<FieldType>();
	}
};

} // namespace db
