#pragma once

#include "../GmeRequest.hpp"

RESPONSE_NS_BEGIN
struct UserLevelMst : public IResponse
{
	struct Data
	{
		explicit Data() :
			level(0), exp(0), actionPoints(0),
			deckCost(0), friendCount(0), addFriendCount(0)
		{}

		uint32_t level, exp, actionPoints, deckCost, friendCount,
			addFriendCount;

		void Serialize(Json::Value& v) const
		{
			v["D9wXQI2V"] = level;
			v["d96tuT2E"] = exp;
			v["0P9X1YHs"] = actionPoints;
			v["ziex06DY"] = deckCost;
			v["oFQ3mbS6"] = friendCount;
			v["GZ2rKW90"] = addFriendCount;
		}
	};

	const char* getGroupName() const override { return "YDv9bJ3s"; }

	std::vector<Data> Mst;

protected:
	size_t getRespCount() const override { return Mst.size(); }

	void SerializeFields(Json::Value& v, size_t i) const override
	{
		Mst.at(i).Serialize(v);
	}
};
RESPONSE_NS_END

