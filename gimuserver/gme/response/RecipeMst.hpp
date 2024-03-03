#pragma once

#include "../GmeRequest.hpp"

RESPONSE_NS_BEGIN
struct ReceipeMst : public IResponse
{
	struct Data
	{
		uint32_t id, itemId, itemCount, karma, unk;
		std::string materials, keyItemIds, unk2, unk, unk3, unk4;

		explicit Data() : id(0), itemId(0), itemCount(0), karma(0) {}

		void Serialize(Json::Value& v) const
		{
			// TODO: we need full decomp to understand this

			v["4HqhTf3a"] = std::to_string(id);
			v["kixHbe54"] = std::to_string(itemId);
			v["wgV86x1q"] = std::to_string(itemCount);
			v["75YV2q1i"] = materials;
			v["HTVh8a65"] = std::to_string(karma);
			v["7dEB1Kwj"] = keyItemIds;
			v["XuJL4pc5"] = unk;
			v["H6k1LIxC"] = unk2;
			v["qA7M9EjP"] = unk3;
			v["SzV0Nps7"] = unk4;
		}
	};

	const char* getGroupName() const override { return "8f0bCciN"; }

	std::vector<Data> Mst;

protected:
	size_t getRespCount() const override { return Mst.size(); }

	void SerializeFields(Json::Value& v, size_t i) const override
	{
		Mst.at(i).Serialize(v);
	}
};
RESPONSE_NS_END
