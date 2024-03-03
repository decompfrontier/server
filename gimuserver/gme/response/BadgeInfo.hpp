#pragma once

#include "../GmeRequest.hpp"

RESPONSE_NS_BEGIN
struct BadgeInfo : public IResponse
{
	const char* getGroupName() const override { return "h23iRjGN"; }

	explicit BadgeInfo() : scenarioNum(0), unitDictNum(0), itemDictNum(0),
		newFrohun(0), dungeonKeyNum(0) {}

	uint32_t scenarioNum, unitDictNum, itemDictNum, newFrohun,
		dungeonKeyNum;
	std::string badgeData;

protected:
	void SerializeFields(Json::Value& v, size_t) const override
	{
		v["u7Htm9P4"] = std::to_string(scenarioNum);
		v["yCLEs28F"] = std::to_string(unitDictNum);
		v["iakSd6m8"] = std::to_string(itemDictNum);
		v["4AjsNu0m"] = std::to_string(newFrohun);
		v["dD40xrsh"] = std::to_string(dungeonKeyNum);
		v["dUujjBBK"] = badgeData;
	}
};
RESPONSE_NS_END
