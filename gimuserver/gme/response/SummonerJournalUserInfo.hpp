#pragma once

#include "../GmeRequest.hpp"

RESPONSE_NS_BEGIN
struct SummonerJournalUserInfo : public IResponse
{
	const char* getGroupName() const override { return "M3dw18eB"; }
	bool isArray() const override { return false; }

	std::string userId;
	uint32_t points;
	uint32_t journalFlag;

	explicit SummonerJournalUserInfo() : points(0), journalFlag(0) {}

protected:
	void SerializeFields(Json::Value& v, size_t) const override
	{
		v["h7eY3sAK"] = userId;
		v["9hH0neGa"] = points;
		v["da365dB8"] = journalFlag;
	}
};
RESPONSE_NS_END
