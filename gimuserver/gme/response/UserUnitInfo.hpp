#pragma once

#include "../GmeRequest.hpp"

RESPONSE_NS_BEGIN
struct UserUnitInfo : public IResponse
{
	struct Data
	{
		std::string userID, FeSkillInfo, element;
		
		uint32_t userUnitID, unitID, leaderSkillID,
			skillID, extraSkillID, eqipItemFrameID, eqipItemFrameID2,
			eqipItemID, equipItemID2, ExtraPassiveSkillID, ExtraPassiveSkillID2,
			AddExtraPassiveSkillID;
		uint32_t unitTypeID, unitLv, exp, totalExp,
			baseHp, addHp, extHp, limitOverHP,
			baseAtk, addAtk, extAtk, limitOverAtk,
			baseDef, addDef, extDef, limitOverDef,
			baseHeal, addHeal, extHeal, limitOverHeal,
			skillLv, extraSkillLv, receiveDate,
			extCnt, newFlg, UnitImgType, FeBP, FeUsedBP,
			FeMaxUsableBP, OmniLevel;

		explicit Data() : userUnitID(0), unitID(0), leaderSkillID(0),
			skillID(0), extraSkillID(0), eqipItemFrameID(0), eqipItemFrameID2(0),
			eqipItemID(0), equipItemID2(0), ExtraPassiveSkillID(0), ExtraPassiveSkillID2(0),
			AddExtraPassiveSkillID(0),
			unitTypeID(0), unitLv(1), exp(0), totalExp(0),
			baseHp(1), addHp(0), extHp(0), limitOverHP(0),
			baseAtk(1), addAtk(0), extAtk(0), limitOverAtk(0),
			baseDef(1), addDef(0), extDef(0), limitOverDef(0),
			baseHeal(1), addHeal(0), extHeal(0), limitOverHeal(0),
			skillLv(0), extraSkillLv(0), receiveDate(0),
			extCnt(0), newFlg(1), UnitImgType(0), FeBP(0), FeUsedBP(0),
			FeMaxUsableBP(0), OmniLevel(0) 
		{
		}

		void Serialize(Json::Value& v) const
		{
			v["h7eY3sAK"] = userID; // str
			v["edy7fq3L"] = std::to_string(userUnitID); // str
			v["pn16CNah"] = std::to_string(unitID); // str
			v["nBTx56W9"] = unitTypeID;
			v["D9wXQI2V"] = unitLv;
			v["d96tuT2E"] = exp;
			v["gQInj3H6"] = totalExp;
			v["e7DK0FQT"] = baseHp;
			v["cuIWp89g"] = addHp;
			v["TokWs1B3"] = extHp;
			v["ISj9u5VL"] = limitOverHP;
			v["67CApcti"] = baseAtk;
			v["RT4CtH5d"] = addAtk;
			v["t4m1RH6Y"] = extAtk;
			v["D6bKH5eV"] = limitOverAtk;
			v["q08xLEsy"] = baseDef;
			v["GcMD0hy6"] = addDef;
			v["e6mY8Z0k"] = extDef;
			v["3CsiQA0h"] = limitOverDef;
			v["PWXu25cg"] = baseHeal;
			v["C1HZr3pb"] = addHeal;
			v["X6jf8DUw"] = extHeal;
			v["XJs2rPx0"] = limitOverHeal;
			v["iNy0ZU5M"] = element;
			v["oS3kTZ2W"] = std::to_string(leaderSkillID); // str
			v["nj9Lw7mV"] = skillID; // str
			v["3NbeC8AB"] = skillLv;
			v["iEFZ6H19"] = std::to_string(extraSkillID); // str
			v["RQ5GnFE2"] = extraSkillLv;
			v["Bvkx8s6M"] = receiveDate; // unix timestamp
			v["5gXxT7LZ"] = extCnt;
			v["0R3qTPK9"] = std::to_string(eqipItemFrameID); // str
			v["Ge8Yo32T"] = std::to_string(eqipItemID); // str
			v["RXfC31FA"] = std::to_string(eqipItemFrameID2); // str
			v["mZA7fH2v"] = std::to_string(equipItemID2); // str
			v["dJNpLc81"] = newFlg;
			//v["DbMVG16I"] = ? ;
			v["cP83zNsv"] = std::to_string(ExtraPassiveSkillID); // str
			v["LjY4DfRg"] = std::to_string(ExtraPassiveSkillID2); // str
			v["T4rewHa9"] = std::to_string(AddExtraPassiveSkillID); // str
			v["2pAyFjmZ"] = UnitImgType;
			v["bFQbZh3x"] = FeBP;
			v["3RgneFpP"] = FeUsedBP;
			v["GIO9DTif"] = FeMaxUsableBP;
			v["Fnxab5CN"] = FeSkillInfo; // str
			v["49sa3sld"] = OmniLevel;

		}
	};

	const char* getGroupName() const override {
		if (overwrite) return "4ceMWH6k";	// Overwrite all units ?
		return "qC2tJs4E";					// Add to existing units ?
	}

	std::vector<Data> Mst;

	bool overwrite = true;

protected:
	size_t getRespCount() const override { return Mst.size(); }

	void SerializeFields(Json::Value& v, size_t i) const override
	{
		Mst.at(i).Serialize(v);
	}
};
RESPONSE_NS_END
