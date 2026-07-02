#pragma once

#include "DatabaseInterface.h"

#include <gimuserver/packets/all.hpp>

#include <string>
#include <type_traits>

namespace db
{

/*!
* Bridges generated packet structs and database rows.
*
* Most persisted user data is eventually read from or written back into packet
* structs, so this interface lets us reuse the packet schema as the database
* mapping layer instead of maintaining separate DTOs. Each packet specialization
* declares which fields are readable, updatable, or insertable, while callers can
* still provide extra cells for lookup keys or database columns that are not part
* of the packet.
*/
template <typename Packet>
class PacketInterfaceFor
{
public:
	/*!
	* Declares which database operations may use a packet field.
	*/
	struct Ops
	{
		bool read = false;
		bool update = false;
		bool insert = false;
	};

	/*!
	* One packet/database field mapping.
	*
	* The field stores the SQL column name plus generated accessors for reading
	* from a packet and writing row values back into a packet.
	*/
	struct Field
	{
		/*!
		* Creates a packet/database field mapping.
		*
		* @param name SQL column name.
		* @param ops Operations this field participates in.
		* @param getter Reads this field from a packet.
		* @param setter Writes this field from a database row into a packet.
		*/
		Field(
			Key name,
			Ops ops,
			Value (*getter)(const Packet& packet),
			void (*setter)(Packet& packet, const Row& row, const Key& key))
			: name(std::move(name)),
			  ops(ops),
			  getter(getter),
			  setter(setter)
		{
		}

		/*!
		* Reads this field from a packet.
		*
		* @param packet Packet to read from.
		* @return Field value as a database value variant.
		*/
		Value get(const Packet& packet) const
		{
			return getter(packet);
		}

		/*!
		* Writes this field from a database row into a packet.
		*
		* @param packet Packet to populate.
		* @param row Database row to read from.
		*/
		void set(Packet& packet, const Row& row) const
		{
			setter(packet, row, name);
		}

		Key name;
		Ops ops;

	private:
		Value (*getter)(const Packet& packet);
		void (*setter)(Packet& packet, const Row& row, const Key& key);
	};

	using Fields = std::vector<Field>;

	PacketInterfaceFor() = delete;

	/*!
	* Reads packet rows from a table.
	*
	* Schema fields marked read become selected columns. Extra cells are passed
	* through to DatabaseInterface and are normally lookup predicates.
	*
	* @param database Database client or transaction to use.
	* @param table SQL table name.
	* @param cells Extra lookup/data cells for the database query.
	* @return Packets populated from matching database rows.
	*/
	static drogon::Task<InterfaceResult<std::vector<Packet>>> read(
		const Database database,
		const std::string table,
		const Cells cells = {})
	{
		Fields filtered;
		for (const auto& field : fields())
		{
			if (field.ops.read)
			{
				filtered.push_back(field);
			}
		}

		const auto rows = co_await DatabaseInterface::read(
			database,
			table,
			mergeCells(filtered, Packet{}, cells));

		std::vector<Packet> packets;
		packets.reserve(rows.data.size());
		for (const auto& row : rows.data)
		{
			Packet packet{};
			for (const auto& field : filtered)
			{
				field.set(packet, row);
			}
			packets.push_back(std::move(packet));
		}

		co_return InterfaceResult<std::vector<Packet>>{
			.data = std::move(packets),
			.affected = rows.affected,
		};
	}

	/*!
	* Updates database rows from a packet.
	*
	* Schema fields marked update become SET assignments. Extra cells are passed
	* through to DatabaseInterface.
	*
	* @param database Database client or transaction to use.
	* @param table SQL table name.
	* @param packet Packet to read update values from.
	* @param cells Extra lookup/data cells for the database update.
	* @return Number of affected rows.
	*/
	static drogon::Task<InterfaceResult<>> update(
		const Database database,
		const std::string table,
		const Packet packet,
		const Cells cells = {})
	{
		Fields filtered;
		for (const auto& field : fields())
		{
			if (field.ops.update)
			{
				filtered.push_back(field);
			}
		}

		co_return co_await DatabaseInterface::update(
			database,
			table,
			mergeCells(filtered, packet, cells));
	}

	/*!
	* Inserts a database row from a packet.
	*
	* Schema fields marked insert become inserted values. Extra cells are passed
	* through to DatabaseInterface.
	*
	* @param database Database client or transaction to use.
	* @param table SQL table name.
	* @param packet Packet to read insert values from.
	* @param cells Extra data cells for the database insert.
	* @return Inserted row, when SQLite inserted one.
	*/
	static drogon::Task<InterfaceResult<Result>> insert(
		const Database database,
		const std::string table,
		const Packet packet,
		const Cells cells = {})
	{
		Fields filtered;
		for (const auto& field : fields())
		{
			if (field.ops.insert)
			{
				filtered.push_back(field);
			}
		}

		co_return co_await DatabaseInterface::insert(
			database,
			table,
			mergeCells(filtered, packet, cells));
	}

private:
	/*!
	* Returns the packet schema specialization.
	*/
	static Fields fields();

	/*!
	* Creates a schema field for one packet member.
	*
	* @param name SQL column name.
	* @param ops Operations this field participates in.
	* @return Field mapping for the member.
	*/
	template <auto Member>
	static Field field(Key name, Ops ops)
	{
		return Field(
			std::move(name),
			ops,
			&PacketInterfaceFor::template get<Member>,
			&PacketInterfaceFor::template set<Member>);
	}

	/*!
	* Reads a packet member as a database value.
	*/
	template <auto Member>
	static Value get(const Packet& packet)
	{
		return packet.*Member;
	}

	/*!
	* Writes a database row value into a packet member.
	*/
	template <auto Member>
	static void set(Packet& packet, const Row& row, const Key& key)
	{
		using T = std::remove_cvref_t<decltype(packet.*Member)>;
		packet.*Member = row[key].as<T>();
	}

	/*!
	* Converts schema fields and caller-provided cells into database cells.
	*
	* Schema fields are always emitted as data cells. Caller cells keep their
	* original use, so callers can provide lookup predicates or extra data values.
	*/
	static Cells mergeCells(
		const Fields& fields,
		const Packet& packet,
		const Cells& cells)
	{
		Cells output;

		for (const auto& field : fields)
		{
			output.push_back({
				.use = Cell::Use::Data,
				.name = field.name,
				.value = field.get(packet),
			});
		}

		for (const auto& cell : cells)
		{
			output.push_back(cell);
		}

		return output;
	}
};

} // namespace db

#include "PacketInterfaceSchemas.hpp"
