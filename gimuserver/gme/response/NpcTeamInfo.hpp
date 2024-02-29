#pragma once

#include "../GmeRequest.hpp"

RESPONSE_NS_BEGIN
struct NpcTeamInfo : public IResponse
{
	std::string userId;
	std::string friendMessage;
	uint32_t lv;

	const char* getGroupName() const override { return nullptr; }

	explicit NpcTeamInfo() : lv(0) {}

	void SerializeFields(Json::Value& v, size_t i) const override
	{
		v["P_USER_ID"] = userId;
		v["P_LV"] = std::to_string(lv);
		v["P_FRIEND_MESSAGE"] = friendMessage;
	}
};
RESPONSE_NS_END
