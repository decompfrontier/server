#pragma once

#include "../GmeRequest.hpp"
#include "GumiLiveInfo.hpp"

// BaseRequest::createUserInfoTag
REQUEST_NS_BEGIN
struct UserInfo : public IRequest
{
	const char* getGroupName() const override { return "IKqx1Cn9"; }

	std::string mInfo;
	std::string userID;
	std::string contactID;
	GumiLiveInfo gumiInfo;
	std::string pointerName;
	int modelChangeCount;
	std::string deviceName;
	std::string targetOS;
	std::string firstDescMstReq;
	std::string noticeMstListRequest;
	std::string buildPlatformID;
	std::string deviceID; // from URLMstRequest

protected:
	void DeserializeFields(const Json::Value& v, size_t) override
	{
		userID = v.get("h7eY3sAK", "").asCString();
		contactID = v.get("90LWtVUN", "").asCString();
		modelChangeCount = v.get("nrg19RGe", 0).asInt();
		deviceName = v.get("iN7buP0j", "").asCString();
		targetOS = v.get("DFY3k6qp", "").asCString();
		buildPlatformID = v.get("j2lk52Be", "").asCString();
		deviceID = v.get("Ma5GnU0H", "").asCString();
		pointerName = v.get("fKSzGDFb", "").asCString();
		firstDescMstReq = v.get("7oV00FeR", "").asCString();
		noticeMstListRequest = v.get("aXf114Oz", "").asCString();
		mInfo = v.get("236dItKo", "").asCString();
		gumiInfo.Deserialize(v);
	}

};
REQUEST_NS_END
