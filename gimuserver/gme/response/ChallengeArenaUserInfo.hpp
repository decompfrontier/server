#pragma once

#include "../GmeRequest.hpp"

RESPONSE_NS_BEGIN
struct ChallengeArenaUserInfo : public IResponse
{
	const char* getGroupName() const override { return "XGmGpmYW"; }

	explicit ChallengeArenaUserInfo() :
		unkint(0), unkint2(0), unkint3(0),
		unkint4(0), unkint5(0), unkint6(0),
		unkint7(0), unkint8(0), unkint9(0)
	{}

	int unkint, unkint2, unkint3, unkint4,
		unkint5, unkint6, unkint7, unkint8,
		unkint9;

	std::string unkstr, unkstr2;

protected:
	void SerializeFields(Json::Value& v, size_t) const override
	{
		// needs to do full decomp to understand this...

		v["h7eY3sAK"] = unkstr;
		v["Nou5bCmm"] = std::to_string(unkint);
		v["AKP8t3xK"] = std::to_string(unkint2);
		v["KAZmxkgy"] = std::to_string(unkint3); // related to rainbowcoins
		v["e34YV1Ey"] = std::to_string(unkint4);
		v["4lH05mQr"] = std::to_string(unkint5);
		v["xZeGgDQe"] = std::to_string(unkint6);
		v["BcIqcWDM"] = std::to_string(unkint7);
		v["fBGCdi8I"] = std::to_string(unkint8);
		v["zf5Ae850"] = std::to_string(unkint9);
		v["outas79f"] = unkstr2;
	}
};
RESPONSE_NS_END
