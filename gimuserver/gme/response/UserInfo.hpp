#pragma once

#include "../GmeRequest.hpp"

RESPONSE_NS_BEGIN
struct UserInfo : public IResponse
{
	const char* getGroupName() const override { return "IKqx1Cn9"; }

	void Serialize(Json::Value& v) const override
	{
		v["h7eY3sAK"] = userID;
		v["B5JQyV8j"] = handleName;
		v["LZ2PfNm4"] = accountID;
		v["4WSu1irc"] = password;
		v["98WfKiyA"] = friendID;
		v["90LWtVUN"] = contactID;
		v["9sQM2XcN"] = std::to_string(tutorialStatus);
		v["sv6BEI8X"] = std::to_string(tutorialEndFlag);
		// "N4XVE1uA" -> userscenarioinfolist
		// "9yVsu21R" -> userspecialscenarioinfolist
		v["nrg19RGe"] = std::to_string(modelChangeCount);
		v["iyJH5k6p"] = codeExpireDate;
		v["y2v7Sd01"] = std::to_string(friendInvitationFlag);
		v["iN7cYU9i"] = std::to_string(earlyBirdEnd);
		v["5MPcr0sp"] = std::to_string(debugMode);
		v["8kN1tgYU"] = encryptIV;
		v["PA0QwZs1"] = encryptedFriendID;
		v["7oV00FeR"] = firstDesc;
		v["23t3D28i"] = dlcUrl;
		v["a37D29iJ"] = std::to_string(featureGate);
		v["32k0ahkD"] = "";
		v["ABh7acL2"] = serviceRequestEndPointParam;
	}

	std::string accountID, userID, friendID;
	std::string password, handleName, contactID;
	int tutorialStatus; // 0 = have to do?, 4 = don't have to do? 12 = triggerable?
	int tutorialEndFlag; // what's this
	int modelChangeCount; // what's this
	std::string codeExpireDate;
	int friendInvitationFlag; // what's this
	long long earlyBirdEnd; // what's this?
	int debugMode; // huh?
	std::string encryptIV; // is this what I think it is?
	std::string encryptedFriendID; // why?
	std::string firstDesc;
	std::string dlcUrl;
	int featureGate; // vortex?
	// "32k0ahkD"
	std::string serviceRequestEndPointParam; // what's this?
};
RESPONSE_NS_END
