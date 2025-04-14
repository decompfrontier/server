#pragma once

#include "../GmeRequest.hpp"

RESPONSE_NS_BEGIN
struct GachaCategory : public IResponse
{
	struct Data
	{
		explicit Data() : id(0), dispOrder(0), startDate(0), endDate(0) {}

		uint32_t id, dispOrder;
		uint64_t startDate, endDate;
		std::string img, gachaIDList;

		void Serialize(Json::Value& v) const
		{
			v["vx9uyQVQ"] = std::to_string(id);
			v["In7lGGLn"] = img; // str
			v["2r4EoNt4"] = std::to_string(dispOrder);
			v["3rCmq58M"] = gachaIDList;
			v["qA7M9EjP"] = std::to_string(startDate); // unix timestamp
			v["SzV0Nps7"] = std::to_string(endDate); // unix timestamp
		}
	};

	const char* getGroupName() const override { return "IBs49NiH"; }

	std::vector<Data> Mst;

protected:
	size_t getRespCount() const override { return Mst.size(); }

	void SerializeFields(Json::Value& v, size_t i) const override
	{
		Mst.at(i).Serialize(v);
	}
};
RESPONSE_NS_END
