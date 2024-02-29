#pragma once

#include "../GmeRequest.hpp"

RESPONSE_NS_BEGIN
struct TownLocationLvMst : public IResponse
{
	const char* getGroupName() const override { return "9ekQ4tZq"; }

	struct Data
	{
		explicit Data() : id(0), lv(0), karma(0) {}

		uint32_t id, lv, karma;
		std::string release_receipe;

		void Serialize(Json::Value& v) const
		{
			v["y9ET7Aub"] = std::to_string(id);
			v["D9wXQI2V"] = std::to_string(lv);
			v["HTVh8a65"] = std::to_string(karma);
			v["rGoJ6Ty9"] = release_receipe;
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
