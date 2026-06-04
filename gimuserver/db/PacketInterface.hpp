#pragma once

#include <drogon/orm/DbClient.h>
#include <gimuserver/packets/all.hpp>

#include <algorithm>
#include <array>
#include <cstdint>
#include <map>
#include <string>
#include <string_view>
#include <stdexcept>
#include <variant>
#include <vector>

/*!
* Packet-to-database bridge for packet structs backed by a SQL table.
*
* Each supported packet type specializes getPacketFields() below to describe
* which database columns can be read from or written to that packet. Callers
* should use the public read/update APIs instead of reaching into the mapping.
*/
template <typename Packet>
class PacketInterfaceFor
{
public:
	using Database = drogon::orm::DbClientPtr;
	using Exception = drogon::orm::DrogonDbException;
	using Result = drogon::orm::Result;
	using Row = drogon::orm::Row;

	/*!
	* This interface is used only through static methods.
	*/
	PacketInterfaceFor() = delete;

	// Used to build a variant that bridges C++ types with SQL bind values for
	// Drogon's ORM layer. Only types currently supported by the packet system
	// are listed; extend as needed.
	using FieldType = std::variant<int32_t, uint32_t, std::string, bool>;

	/*!
	* Optional owning list of database columns to operate on.
	*
	* Empty means "use every mapped field that is valid for the operation".
	* updateFromPacket uses this to patch a small subset of a packet without
	* first hydrating the full packet from the database.
	*/
	using Columns = std::vector<std::string_view>;

	/*!
	* Maps one database column to one packet field.
	*
	* A null read pointer means the column is write-only. A null write pointer
	* means the column is read-only. This lets identity columns participate in
	* SELECT queries without being overwritten by UPDATE queries.
	*/
	struct PacketField
	{
		// The literal name of the column in the database table (e.g., "username").
		std::string_view column;

		// Function pointer that reads a column from a DB row and assigns it to a struct member.
		void (*read)(const Row& row, Packet& packet, const std::string_view column);

		// Function pointer that writes a struct member's value into a type-safe parameter vector.
		void (*write)(const Packet& packet, std::vector<FieldType>& fields);
	};

	/*!
	* Owning field list returned by getPacketFields().
	*/
	using PacketFields = std::vector<PacketField>;

private:
	// Intentionally undefined for unsupported packet types.
	static PacketFields getPacketFields();

	/*!
	* Filters a packet field mapping by database column name.
	* @param fields Complete packet-to-column mapping for the packet type.
	* @param columns Optional list of columns to keep. Empty keeps all fields.
	*/
	static PacketFields filterPacketFields(
		const PacketFields& fields,
		const Columns& columns)
	{
		PacketFields selected;

		if (columns.empty())
		{
			selected.reserve(fields.size());
			selected.insert(selected.end(), fields.begin(), fields.end());
			return selected;
		}

		selected.reserve(columns.size());
		for (const auto& column : columns)
		{
			const auto it = std::find_if(
				fields.begin(),
				fields.end(),
				[column](const PacketField& field) {
					return field.column == column;
				});

			if (it == fields.end())
			{
				LOG_ERROR << "Unknown packet column '" << column << "'";
				return {};
			}
			if (!it->write)
			{
				LOG_ERROR << "Packet column '" << column << "' is read-only";
				return {};
			}

			selected.push_back(*it);
		}

		return selected;
	}

	// Field declarations use these helpers when specializing getPacketFields().
	// Reads one typed DB column into one packet member.
	template <typename T, T Packet::* Field>
	static void readField(const Row& row, Packet& packet, const std::string_view column)
	{
		packet.*Field = row[std::string(column)].as<T>();
	}

	// Writes one packet member into the SQL bind-value list.
	template <typename T, T Packet::* Field>
	static void writeField(const Packet& packet, std::vector<FieldType>& fields)
	{
		fields.push_back(packet.*Field);
	}

	/*!
	* Iterates the field mapping to extract every readable column from a DB row into a packet.
	* Fields whose read function pointer is null (write-only fields) are skipped.
	* @param row The source database row containing the query results.
	* @param packet The target packet instance being populated.
	* @param fields The field mapping guiding the extraction.
	*/
	static void readFields(
		const Row& row,
		Packet& packet,
		const PacketFields& fields)
	{
		for (const auto& field : fields)
		{
			// Some fields may be write-only, so we check if the read function is
			// defined before calling it.
			if (field.read)
			{
				field.read(row, packet, field.column);
			}
		}
	}

	/*!
	* Generates a comma-separated list of column names for a SELECT statement.
	* Only readable columns (those with a non-null read function pointer) are included.
	* @param fields The field mapping guiding the column list.
	*/
	static std::string selectList(const PacketFields& fields)
	{
		std::string sql;
		for (const auto& field : fields)
		{
			// Only include fields that have a read function defined.
			if (!field.read)
			{
				continue;
			}
			if (!sql.empty())
			{
				sql += ", ";
			}
			sql += field.column;
		}
		return sql;
	}

	/*!
	* Serializes every writable field into a type-safe SQL parameter vector.
	* @param packet The packet instance containing the source values.
	* @param fields The field mapping guiding serialization.
	*/
	static std::vector<FieldType> writeFields(
		const Packet& packet,
		const PacketFields& fields)
	{
		std::vector<FieldType> f;
		f.reserve(fields.size());

		for (const auto& field : fields)
		{
			// Some fields may be read-only, so we check if the write function
			// is defined before calling it.
			if (field.write)
			{
				field.write(packet, f);
			}
		}
		return f;
	}

	/*!
	* Generates assignment placeholders for an UPDATE SET clause.
	* Only writable columns (those with a non-null write function pointer) are included.
	* @param fields The field mapping guiding the clause.
	*/
	static std::string updateList(const PacketFields& fields)
	{
		std::string sql;
		size_t idx = 1;

		for (const auto& field : fields)
		{
			// Only include fields that have a write function defined.
			if (!field.write)
			{
				continue;
			}
			if (!sql.empty())
			{
				sql += ", ";
			}
			sql += std::string(field.column) + " = $" + std::to_string(idx++);
		}
		return sql;
	}

	// Ordered criteria keeps generated WHERE placeholders and bound values in sync.
	using Criteria = std::map<std::string_view, FieldType>;

	/*!
	* Generates a deterministic WHERE clause from criteria columns.
	* @param criteria Map containing lookup constraints, automatically sorted alphabetically by key.
	* @param from The SQL placeholder index offset ($1, $2, etc.) to start printing at.
	*/
	static std::string whereClause(const Criteria& criteria, const size_t from = 1)
	{
		if (criteria.empty())
		{
			return "";
		}
		std::string sql = " WHERE ";
		size_t index = from;

		for (auto it = criteria.begin(); it != criteria.end(); ++it)
		{
			if (it != criteria.begin())
			{
				sql += " AND ";
			}
			sql += std::string(it->first) + " = $" + std::to_string(index++);
		}
		return sql;
	}

public:
	/*!
	* Updates the given table using packet field mappings.
	*
	* By default this writes every writable field from the packet. Pass Columns
	* to update only a named subset; this is useful for patching a single field
	* without reading the whole packet first or risking default values from other
	* packet members being written back to the database.
	*
	* @param db Database connection.
	* @param table Table name to update.
	* @param criteria WHERE clause constraints (column -> value map).
	* @param packet Source packet to update the table with.
	* @param columns Optional database column filter. Empty updates every writable field.
	*/
	static drogon::Task<Result> updateFromPacket(
		const Database db,
		const std::string_view table,
		const Criteria criteria,
		const Packet packet,
		const Columns columns = {})
	{
		const auto fields = filterPacketFields(getPacketFields(), columns);
		const auto clause = updateList(fields);
		const auto values = writeFields(packet, fields);

		// Fail-fast if
		// - No database connection
		// - No table name provided
		// - No criteria provided (to prevent accidental updates)
		// - No fields to update (to prevent invalid SQL)
		// - No values to bind (to prevent invalid SQL)
		if (!db || table.empty() || criteria.empty() || clause.empty() || values.empty())
		{
			LOG_ERROR << "Invalid updateFromPacket call: "
				<< "db=" << static_cast<bool>(db)
				<< ", table='" << table << "'"
				<< ", criteria=" << criteria.size()
				<< ", clause_empty=" << clause.empty()
				<< ", values=" << values.size();
			throw std::invalid_argument("Invalid updateFromPacket call");
		}

		const auto sql = "UPDATE " + std::string(table) + " SET " + clause +
			whereClause(criteria, /*from=*/values.size() + 1) + ";";

		auto binder = *db << sql;

		// Bind payload parameters first ($1 to $N).
		for (const auto& value : values)
		{
			std::visit([&binder](const auto& v) -> void {
				binder << v;
			}, value);
		}

		// Bind lookup criteria parameters ($N+1 onwards).
		for (const auto& [column, value] : criteria)
		{
			std::visit([&binder](const auto& v) -> void {
				binder << v;
			}, value);
		}

		auto result = co_await drogon::orm::internal::SqlAwaiter(std::move(binder));
		co_return result;
	}

	/*!
	* Reads the first matching row from the given table into a packet.
	*
	* This is a convenience wrapper over readToPackets for call sites that only
	* care about fetching one row.
	*
	* @param db Database connection.
	* @param table Table name to query.
	* @param criteria WHERE clause constraints (column -> value map).
	* @param packet Output packet to populate with.
	*/
	static drogon::Task<Result> readToPacket(
		const Database db,
		const std::string_view table,
		const Criteria criteria,
		Packet& packet)
	{
		std::vector<Packet> packets;
		packets.emplace_back(packet);

		auto result = co_await readToPackets(db, table, criteria, packets);
		if (result.empty())
		{
			co_return result;
		}

		packet = std::move(packets.front());
		co_return result;
	}

	/*!
	* Reads every matching row from the given table into packet instances.
	*
	* The input vector may grow if the query returns more rows than it
	* already has, but it is never shrunk by this function.
	*
	* @param db Database connection.
	* @param table Table name to query.
	* @param criteria WHERE clause constraints (column -> value map).
	* @param packets Packet collection to update in place with query results.
	*/
	static drogon::Task<Result> readToPackets(
		const Database db,
		const std::string_view table,
		const Criteria criteria,
		std::vector<Packet>& packets)
	{
		const auto fields = getPacketFields();
		const auto columnsList = selectList(fields);

		// Fail-fast if:
		// - No database connection
		// - No table name provided
		// - No criteria provided (to prevent accidental reads)
		// - No fields to read (to prevent invalid SQL)
		if (!db || table.empty() || criteria.empty() || columnsList.empty())
		{
			LOG_ERROR << "Invalid readToPackets call: "
				<< "db=" << static_cast<bool>(db)
				<< ", table='" << table << "'"
				<< ", criteria=" << criteria.size()
				<< ", columns_empty=" << columnsList.empty();
			throw std::invalid_argument("Invalid readToPackets call");
		}

		const std::string sql = "SELECT " + columnsList +
			" FROM " + std::string(table) +
			whereClause(criteria, /*from=*/1) + ";";

		auto binder = *db << sql;

		for (const auto& [column, value] : criteria)
		{
			std::visit([&binder](const auto& v) -> void {
				binder << v;
			}, value);
		}

		auto result = co_await drogon::orm::internal::SqlAwaiter(std::move(binder));
		if (result.size() > packets.size())
		{
			packets.resize(result.size());
		}

		size_t rowIndex = 0;
		for (const auto& row : result)
		{
			readFields(row, packets[rowIndex], fields);
			++rowIndex;
		}

		co_return result;
	}
};

/*!
* Specializations for supported packet types go here. Each specialization defines a static
* constexpr array of PacketField structs that map database columns to packet members.
*/

template <>
inline PacketInterfaceFor<LoginInfoResp>::PacketFields PacketInterfaceFor<LoginInfoResp>::getPacketFields()
{
	static constexpr std::array<PacketField, 4> fields = {{
		{ "id", &readField<std::string, &LoginInfoResp::user_id>, nullptr },
		{ "gumi_user_id", &readField<std::string, &LoginInfoResp::gumi_live_userid>, nullptr },
		{
			"username",
			&readField<std::string, &LoginInfoResp::handle_name>,
			&writeField<std::string, &LoginInfoResp::handle_name>
		},
		{
			"tutorial_status",
			&readField<int32_t, &LoginInfoResp::tutorial_status>,
			&writeField<int32_t, &LoginInfoResp::tutorial_status>
		},
	}};

	return { fields.begin(), fields.end() };
}

template <>
inline PacketInterfaceFor<UserTeamInfo>::PacketFields PacketInterfaceFor<UserTeamInfo>::getPacketFields()
{
	static constexpr std::array<PacketField, 5> fields = {{
		{ "id", &readField<std::string, &UserTeamInfo::user_id>, nullptr },
		{
			"level",
			&readField<int32_t, &UserTeamInfo::level>,
			&writeField<int32_t, &UserTeamInfo::level>
		},
		{
			"max_unit_count",
			&readField<int32_t, &UserTeamInfo::max_unit_count>,
			&writeField<int32_t, &UserTeamInfo::max_unit_count>
		},
		{
			"max_warehouse_count",
			&readField<int32_t, &UserTeamInfo::warehouse_count>,
			&writeField<int32_t, &UserTeamInfo::warehouse_count>
		},
		{
			"active_deck",
			&readField<int32_t, &UserTeamInfo::active_deck>,
			&writeField<int32_t, &UserTeamInfo::active_deck>
		},
	}};

	return { fields.begin(), fields.end() };
}

template <>
inline PacketInterfaceFor<UserUnitInfo>::PacketFields PacketInterfaceFor<UserUnitInfo>::getPacketFields()
{
	static constexpr std::array<PacketField, 12> fields = {{
		{ "user_unit_id", &readField<uint32_t, &UserUnitInfo::user_unit_id>, nullptr },
		{ "user_id", &readField<std::string, &UserUnitInfo::user_id>, nullptr },
		{
			"unit_id",
			&readField<uint32_t, &UserUnitInfo::unit_id>,
			&writeField<uint32_t, &UserUnitInfo::unit_id>
		},
		{
			"unit_type_id",
			&readField<uint32_t, &UserUnitInfo::unit_type_id>,
			&writeField<uint32_t, &UserUnitInfo::unit_type_id>
		},
		{
			"base_hp",
			&readField<uint32_t, &UserUnitInfo::base_hp>,
			&writeField<uint32_t, &UserUnitInfo::base_hp>
		},
		{
			"base_atk",
			&readField<uint32_t, &UserUnitInfo::base_atk>,
			&writeField<uint32_t, &UserUnitInfo::base_atk>
		},
		{
			"base_def",
			&readField<uint32_t, &UserUnitInfo::base_def>,
			&writeField<uint32_t, &UserUnitInfo::base_def>
		},
		{
			"base_rec",
			&readField<uint32_t, &UserUnitInfo::base_rec>,
			&writeField<uint32_t, &UserUnitInfo::base_rec>
		},
		{
			"ext_hp",
			&readField<uint32_t, &UserUnitInfo::ext_hp>,
			&writeField<uint32_t, &UserUnitInfo::ext_hp>
		},
		{
			"ext_atk",
			&readField<uint32_t, &UserUnitInfo::ext_atk>,
			&writeField<uint32_t, &UserUnitInfo::ext_atk>
		},
		{
			"ext_def",
			&readField<uint32_t, &UserUnitInfo::ext_def>,
			&writeField<uint32_t, &UserUnitInfo::ext_def>
		},
		{
			"ext_rec",
			&readField<uint32_t, &UserUnitInfo::ext_rec>,
			&writeField<uint32_t, &UserUnitInfo::ext_rec>
		},
	}};

	return { fields.begin(), fields.end() };
}
