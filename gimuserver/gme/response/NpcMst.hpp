#pragma once

#include "../GmeRequest.hpp"
#include "NpcPartyInfo.hpp"
#include "NpcTeamInfo.hpp"
#include "NpcUnitInfo.hpp"

RESPONSE_NS_BEGIN
struct NpcMst : public IResponse
{
	const char* getGroupName() const override { return "hV5vWu6C"; }

	struct Data
	{
		explicit Data() : id(0), arena_rank_id(0) {}

		uint32_t id, arena_rank_id;
		std::string handle_name;
		NpcTeamInfo team;
		NpcUnitInfo units;
		NpcPartyInfo parties;

		void Serialize(Json::Value& v) const
		{
			Json::StreamWriterBuilder b;
			b["indentation"] = "";
			b["commentStyle"] = "None";
			b["emitUTF8"] = true;

			v["7zyHb5h9"] = std::to_string(id);
			v["B5JQyV8j"] = handle_name; // str
			v["JmFn3g9t"] = std::to_string(arena_rank_id);

			{
				Json::Value i;
				team.Serialize(i);
				v["g94bDiaS"] = Json::writeString(b, i);
			}

			{
				Json::Value i;
				parties.Serialize(i);
				v["oPsmRC18"] = Json::writeString(b, i);
			}

			{
				Json::Value i;
				units.Serialize(i);
				v["bS9s4GCp"] = Json::writeString(b, i);
			}
		}
	};

	std::vector<Data> Mst;

protected:
	size_t getRespCount() const override { return Mst.size(); }

	void SerializeFields(Json::Value& v, size_t i) const override
	{
		Mst.at(i).Serialize(v);
	}
};
RESPONSE_NS_END
