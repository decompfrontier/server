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
		v["Nou5bCmm"] = unkint;
		v["AKP8t3xK"] = unkint2;
		v["KAZmxkgy"] = unkint3; // related to rainbowcoins
		v["e34YV1Ey"] = unkint4;
		v["4lH05mQr"] = unkint5;
		v["xZeGgDQe"] = unkint6;
		v["BcIqcWDM"] = unkint7;
		v["fBGCdi8I"] = unkint8;
		v["zf5Ae850"] = unkint9;
		v["outas79f"] = unkstr2;
	}
};
RESPONSE_NS_END
