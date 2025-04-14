#pragma once

#include "../GmeRequest.hpp"

RESPONSE_NS_BEGIN
struct GachaEffectMst : public IResponse
{
	const char* getGroupName() const override { return "Pf97SzVw"; }

	struct Data
	{
		explicit Data() : id(0), gatcha_id(0), rare(0), rate(0.0f) {}

		uint32_t id, gatcha_id, rare;
		float rate;
		std::string effect_before, effect_after, effect;

		void Serialize(Json::Value& v) const
		{
			v["u0vkt9yH"] = std::to_string(id);
			v["7Ffmi96v"] = std::to_string(gatcha_id);
			v["7ofj5xa1"] = std::to_string(rare);
			v["ug9xV4Fz"] = std::to_string(rate); // float
			v["7ZNcmYS2"] = effect_before; // str
			v["tj0i9JhC"] = effect_after; // str
			v["YTx3c1jQ"] = effect; // str
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
