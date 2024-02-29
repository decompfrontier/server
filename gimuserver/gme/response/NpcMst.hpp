#pragma once

#include "../GmeRequest.hpp"

RESPONSE_NS_BEGIN
struct NpcMst : public IResponse
{
	const char* getGroupName() const override { return "hV5vWu6C"; }

	struct Data
	{
		explicit Data() : id(0), arena_rank_id(0) {}

		uint32_t id, arena_rank_id;
		std::string handle_name, team_info, party_info, unit_info;

		void Serialize(Json::Value& v) const
		{
			v["7zyHb5h9"] = id;
			v["B5JQyV8j"] = handle_name; // str
			v["JmFn3g9t"] = arena_rank_id;
			v["g94bDiaS"] = team_info;
			v["oPsmRC18"] = party_info;
			v["bS9s4GCp"] = unit_info;
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
