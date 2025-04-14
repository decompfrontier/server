#pragma once

#include "../GmeRequest.hpp"

RESPONSE_NS_BEGIN
struct ExtraPassiveSkillMst : public IResponse
{
	struct Data
	{
		explicit Data() : id(0), target(0), priority(0), rare(0),
			groupID(0), skillType(0) {}

		uint32_t id, target, priority, rare, groupID,
			skillType;
		std::string skillName, skillNameS, processParam,
			termParam, description, processID;

		void Serialize(Json::Value& v) const
		{
			v["cP83zNsv"] = std::to_string(id);
			v["0nxpBDz2"] = skillName;
			v["UVN5bGoD"] = skillNameS;
			v["outas79f"] = std::to_string(groupID);
			v["yu18xScw"] = std::to_string(priority);
			v["hjAy9St3"] = processID;
			v["2Smu5Mtq"] = processParam;
			v["s2gM3deu"] = termParam;
			v["moWQ30GH"] = std::to_string(target);
			v["qp37xTDh"] = description;
			v["h6UL9A1B"] = std::to_string(skillType);
			v["7ofj5xa1"] = std::to_string(rare);
		}
	};

	const char* getGroupName() const override { return "?"; }

	std::vector<Data> Mst;

protected:
	size_t getRespCount() const override { return Mst.size(); }

	void SerializeFields(Json::Value& v, size_t i) const override
	{
		Mst.at(i).Serialize(v);
	}
};
RESPONSE_NS_END
