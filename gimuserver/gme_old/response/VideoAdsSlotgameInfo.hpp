#pragma once

#include "../GmeRequest.hpp"
#include "SlotGameInfo.hpp"
#include "SlotGameReelInfo.hpp"
#include "SlotGamePictureInfo.hpp"
#include "VideoAdsSlotGameStandInfo.hpp"

RESPONSE_NS_BEGIN
struct VideoAdsSlotgameInfo : public IResponse
{
	explicit VideoAdsSlotgameInfo() {}

	const char* getGroupName() const override { return "mebW7mKD"; }

	SlotGameInfo gameInfo;
	SlotGameReelInfo reelInfo;
	SlotGamePictureInfo pictureInfo;
	VideoAdsSlotGameStandInfo gameStandInfo;

	void CacheFields()
	{
		Json::StreamWriterBuilder b;
		b["indentation"] = "";
		b["commentStyle"] = "None";
		b["emitUTF8"] = true;

		{

			Json::Value r;
			gameInfo.Serialize(r);
			cacheGameInfo = Json::writeString(b, r);
		}

		{

			Json::Value r;
			reelInfo.Serialize(r);
			cacheReelInfo = Json::writeString(b, r);
		}

		{
			Json::Value r;
			pictureInfo.Serialize(r);
			cachePictureInfo = Json::writeString(b, r);
		}
	}

protected:
	std::string cacheGameInfo, cacheReelInfo, cachePictureInfo;

	void SerializeFields(Json::Value& v, size_t i) const override
	{
		v["C38FmiUn"] = cacheGameInfo;
		v["iW62Scdg"] = cacheReelInfo;
		v["rY6j0Jvs"] = cachePictureInfo;

		{
			Json::StreamWriterBuilder b;
			b["indentation"] = "";
			b["commentStyle"] = "None";
			b["emitUTF8"] = true;

			Json::Value r;
			gameStandInfo.Serialize(r);
			v["tclBMiv2"] = Json::writeString(b, r);
		}
	}
};
RESPONSE_NS_END

