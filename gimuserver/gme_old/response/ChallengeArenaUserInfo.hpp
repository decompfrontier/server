#pragma once

#include "../GmeRequest.hpp"

RESPONSE_NS_BEGIN
struct ChallengeArenaUserInfo : public IResponse
{
	const char* getGroupName() const override { return "XGmGpmYW"; }

	explicit ChallengeArenaUserInfo() :
		unkint(0), unkint2(0), rainbowCoins(0),
		unkint4(0), unkint5(0), leagueId(0),
		unkint7(0), unkint8(0), unkint9(0)
	{}

	uint32_t unkint, unkint2, rainbowCoins, unkint4,
		unkint5, leagueId, unkint7, unkint8,
		unkint9;

	std::string unkstr, unkstr2;

protected:
	void SerializeFields(Json::Value& v, size_t) const override
	{
		// TODO: we barely have infos inside the decomp about this class atm...

		v["h7eY3sAK"] = unkstr;
		v["Nou5bCmm"] = std::to_string(unkint);
		v["AKP8t3xK"] = std::to_string(unkint2);
		v["KAZmxkgy"] = std::to_string(rainbowCoins); // related to rainbowcoins
		v["e34YV1Ey"] = std::to_string(unkint4);
		v["4lH05mQr"] = std::to_string(unkint5);
		v["xZeGgDQe"] = std::to_string(leagueId);
		v["BcIqcWDM"] = std::to_string(unkint7);
		v["fBGCdi8I"] = std::to_string(unkint8);
		v["zf5Ae850"] = std::to_string(unkint9);
		v["outas79f"] = unkstr2;
	}
};
RESPONSE_NS_END
