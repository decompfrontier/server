#pragma once

#include "../GmeRequest.hpp"

RESPONSE_NS_BEGIN
struct UserPartyDeckInfo : public IResponse
{
	struct Data
	{
		uint32_t deckType, deckNum, memberType, dispOrder, userUnitID;

		explicit Data() : deckType(0), deckNum(0), memberType(0), dispOrder(0),
			userUnitID(0) {}

		void Serialize(Json::Value& v) const
		{
			v["U9ABSYEp"] = deckType;
			v["zsiAn9P1"] = deckNum;
			v["edy7fq3L"] = std::to_string(userUnitID);
			v["gr48vsdJ"] = std::to_string(memberType);
			v["XuJL4pc5"] = std::to_string(dispOrder);
		}
	};

	const char* getGroupName() const override { return "dX7S2Lc1"; }

	std::vector<Data> Mst;

protected:
	size_t getRespCount() const override { return Mst.size(); }

	void SerializeFields(Json::Value& v, size_t i) const override
	{
		Mst.at(i).Serialize(v);
	}
};
RESPONSE_NS_END
