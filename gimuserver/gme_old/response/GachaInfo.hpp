#pragma once

#include "../GmeRequest.hpp"

RESPONSE_NS_BEGIN
struct GachaInfo : public IResponse
{
	struct Data
	{
		explicit Data() : id(0), braveCoin(0), resummonGachaFlag(0), gachaType(0), priority(0),
			needFriendPoint(0), onceDayFlag(0), gachaDetailID(0), gachaGroupID(0) {}

		uint32_t id, braveCoin, resummonGachaFlag, gachaType, priority,
			needFriendPoint, onceDayFlag, gachaDetailID, gachaGroupID;
		std::string btnImg, bgImg, endDate, captionMsg, detailMsg, gachaName,
			campaignInfo, startDate, startHour, endHour, doorImg, description,
			commentMsg, contentsBannerImg;

		void Serialize(Json::Value& v) const
		{
			v["7Ffmi96v"] = std::to_string(id);
			v["8HM3v3gg"] = campaignInfo;
			v["03UGMHxF"] = std::to_string(braveCoin);
			v["W9ABuJj2"] = btnImg;
			v["1Dg0vUX3"] = bgImg;
			v["3sdHQb69"] = captionMsg;
			v["W2c9g0Je"] = detailMsg;
			v["SzV0Nps7"] = endDate;
			v["a632bkyQ"] = resummonGachaFlag; // always 0?
			v["4N27mkt1"] = gachaName;
			v["S1oz60Hc"] = std::to_string(gachaType); // 1,2,200?
			v["yu18xScw"] = std::to_string(priority);
			v["qA7M9EjP"] = startDate;
			v["2HY3jpgu"] = startHour;
			v["v9TR3cDz"] = endHour;
			v["J3stQ7jd"] = std::to_string(needFriendPoint);
			v["4tswNoV9"] = std::to_string(onceDayFlag);
			v["uKYf13AH"] = doorImg;
			v["qp37xTDh"] = description;
			v["TCnm1F4v"] = std::to_string(gachaGroupID);
			v["gVSj32QH"] = commentMsg;

			if (gachaDetailID > 0)
				v["8Z9CYQDq"] = std::to_string(gachaDetailID);
			else
				v["8Z9CYQDq"] = "";

			v["sA9dDAqB"] = contentsBannerImg;
		}
	};

	const char* getGroupName() const override { return "1IR86sAv"; }

	std::vector<Data> Mst;

protected:
	size_t getRespCount() const override { return Mst.size(); }

	void SerializeFields(Json::Value& v, size_t i) const override
	{
		Mst.at(i).Serialize(v);
	}
};
RESPONSE_NS_END
