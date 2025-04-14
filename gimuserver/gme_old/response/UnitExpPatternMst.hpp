#pragma once

#include "../GmeRequest.hpp"

RESPONSE_NS_BEGIN
struct UnitExpPatternMst : public IResponse
{
	const char* getGroupName() const override { return "JYFGe9y6"; }
	bool isArray() const override { return true; }

	struct Data
	{
		uint32_t id;
		uint32_t lv;
		uint64_t needExp;

		void Serialize(Json::Value& v) const
		{
			v["5UvTp7q1"] = std::to_string(id);
			v["D9wXQI2V"] = std::to_string(lv);
			v["grKTq15A"] = std::to_string(needExp);
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
