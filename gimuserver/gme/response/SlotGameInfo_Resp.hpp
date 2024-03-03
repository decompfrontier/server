#pragma once

#include "../GmeRequest.hpp"
#include "SlotGameInfo.hpp"
#include "SlotGamePictureInfo.hpp"

RESPONSE_NS_BEGIN
struct SlotGameInfoR : public IResponse
{
	SlotGameInfo info;
	SlotGamePictureInfo pictures;

	const char* getGroupName() const override { return "C38FmiUn"; }

protected:
	void SerializeFields(Json::Value& v, size_t i) const override
	{
		Json::StreamWriterBuilder b;
		b["indentation"] = "";
		b["commentStyle"] = "None";
		b["emitUTF8"] = true;

		{

			Json::Value r;
			info.Serialize(r);
			v["C38FmiUn"] = Json::writeString(b, r);
		}

		{

			Json::Value r;
			pictures.Serialize(r);
			v["rY6j0Jvs"] = Json::writeString(b, r);
		}
	}
};
RESPONSE_NS_END
