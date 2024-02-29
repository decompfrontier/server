#pragma once

#include "../GmeRequest.hpp"

RESPONSE_NS_BEGIN
struct SlotGameInfo : public IResponse
{
	uint32_t id;
	std::string name;
	std::string reelPos;
	std::string useMedal;
	std::string slotHelpUrl;
	std::string slotImage;

	const char* getGroupName() const override { return nullptr; }
	bool isArray() const override { return false; }

protected:
	void SerializeFields(Json::Value& v, size_t i) const override
	{
		v["zS45RFGb"] = std::to_string(id);
		v["I1Cki7Pb"] = name;
		v["h1PSnk5t"] = reelPos;
		v["b5yeVr61"] = useMedal;
		v["jsRoN50z"] = slotHelpUrl;
		v["TX98PnpE"] = slotImage;
	}
};
RESPONSE_NS_END
