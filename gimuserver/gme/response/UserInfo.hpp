#pragma once

#include "../GmeRequest.hpp"

RESPONSE_NS_BEGIN
struct UserInfo : public IResponse
{
	const char* getGroupName() const override { return "IKqx1Cn9"; }
	explicit UserInfo() : tutorialStatus(0), tutorialEndFlag(0), modelChangeCount(0)
		, codeExpireDate(0)
		, friendInvitationFlag(0), earlyBirdEnd(0), debugMode(0),
		featureGate(0)
	{
	}

	std::string accountID, userID, friendID;
	std::string password, handleName, contactID; // contact id? password?
	int tutorialStatus; // 0 = have to do?, 4 = don't have to do? 12 = triggerable?
	int tutorialEndFlag; // what's this
	int modelChangeCount; // what's this
	time_t codeExpireDate;
	int friendInvitationFlag; // what's this
	long long earlyBirdEnd; // what's this?
	int debugMode;
	std::string encryptIV; // is this what I think it is?
	std::string encryptedFriendID; // why?
	std::string firstDesc; // ?
	std::string dlcUrl; // ?
	int featureGate; // vortex?
	// "32k0ahkD"
	std::string serviceRequestEndPointParam; // what's this?

protected:
	void SerializeFields(Json::Value& v, size_t) const override
	{
		v["h7eY3sAK"] = userID;
		v["B5JQyV8j"] = handleName;
		v["LZ2PfNm4"] = accountID;
		v["4WSu1irc"] = password;
		v["98WfKiyA"] = friendID;
		v["90LWtVUN"] = contactID;
		v["9sQM2XcN"] = std::to_string(tutorialStatus);
		v["sv6BEI8X"] = std::to_string(tutorialEndFlag);
		v["N4XVE1uA"] = ""; // TODO: userscenarioinfolist
		v["9yVsu21R"] = ""; // TODO: userspecialscenarioinfolist
		v["nrg19RGe"] = std::to_string(modelChangeCount);
		v["iyJH5k6p"] = std::to_string(codeExpireDate);
		v["y2v7Sd01"] = std::to_string(friendInvitationFlag);
		v["iN7cYU9i"] = std::to_string(earlyBirdEnd);
		v["5MPcr0sp"] = std::to_string(debugMode);
		v["8kN1tgYU"] = encryptIV;
		v["PA0QwZs1"] = encryptedFriendID;
		v["7oV00FeR"] = firstDesc;

		if (!dlcUrl.empty())
			v["23t3D28i"] = dlcUrl;

		v["a37D29iJ"] = std::to_string(featureGate);
		v["32k0ahkD"] = "773c9af44721a014c7ed"; // TODO: discover what's this

		if (!serviceRequestEndPointParam.empty())
			v["ABh7acL2"] = serviceRequestEndPointParam;
	}
};
RESPONSE_NS_END
