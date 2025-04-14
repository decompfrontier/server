#pragma once

#include "../GmeRequest.hpp"

RESPONSE_NS_BEGIN
struct DungeonKeyMst : public IResponse
{
	const char* getGroupName() const override { return "4NG79sX1"; }

	struct Data
	{
		explicit Data() : id(1), dungeonId(0), limitSec(0), possessionLimit(0),
			distributeCount(0), distributeFlag(0), state(0) {}

		uint32_t id, dungeonId, limitSec, possessionLimit, distributeCount,
			distributeFlag, state;
		std::string name, thumImg, openImg, closeImg, usagePattern;

		void Serialize(Json::Value& v) const
		{
			v["16KMNJLb"] = std::to_string(id);
			v["BM29ZgnK"] = name;
			v["MHx05sXt"] = std::to_string(dungeonId);
			v["M2cv6dum"] = thumImg;
			v["VX0j1fni"] = openImg;
			v["9unNZ6b0"] = closeImg;
			v["i9sBW8uD"] = std::to_string(limitSec);
			v["N7I9vYZb"] = std::to_string(possessionLimit);

			if (distributeCount > 0)
				v["khsb74Nq"] = std::to_string(distributeCount);

			v["EK5I6MQ9"] = std::to_string(distributeFlag);
			v["CR6aKWg8"] = usagePattern;
			v["j0Uszek2"] = std::to_string(state);
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
