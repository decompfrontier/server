#pragma once

#include "../GmeRequest.hpp"

RESPONSE_NS_BEGIN
struct NpcUnitInfo : public IResponse
{
	struct Data
	{
		uint32_t id;
		uint32_t party_id;
		uint32_t type;
		uint32_t lv;
		uint32_t hp;
		uint32_t atk;
		uint32_t def;
		uint32_t hel;
		uint32_t skill_id;
		uint32_t skill_lv;
		uint32_t equip_item_id;

		explicit Data() : id(0), party_id(0), type(0), lv(0), hp(0),
			atk(0), def(0), hel(0), skill_id(0), skill_lv(0), equip_item_id(0) {}

		void Serialize(Json::Value& v) const
		{
			v["P_UNIT_ID"] = std::to_string(id);
			v["P_USER_UNIT_ID"] = std::to_string(party_id);
			v["P_UNIT_TYPE_ID"] = std::to_string(type);
			v["P_LV"] = std::to_string(lv);
			v["P_HP"] = std::to_string(hp);
			v["P_ATK"] = std::to_string(atk);
			v["P_DEF"] = std::to_string(def);
			v["P_HEL"] = std::to_string(hel);
			v["P_SKILL_LV"] = std::to_string(skill_lv);

			if (skill_id > 0)
				v["P_SKILL_ID"] = std::to_string(skill_id);
			else
				v["P_SKILL_ID"] = "";

			v["P_EQP_ITEM_ID"] = std::to_string(equip_item_id);
		}
	};

	const char* getGroupName() const override { return nullptr; }

	std::vector<Data> Mst;

protected:
	size_t getRespCount() const override { return Mst.size(); }

	void SerializeFields(Json::Value& v, size_t i) const override
	{
		Mst.at(i).Serialize(v);
	}
};
RESPONSE_NS_END
