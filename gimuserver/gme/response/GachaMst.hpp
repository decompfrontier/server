#pragma once

#include "../GmeRequest.hpp"

RESPONSE_NS_BEGIN
struct GachaMst : public IResponse
{
	const char* getGroupName() const override { return "5Y4GJeo3"; }

	struct Data
	{
		explicit Data() : id(0), type(0), priority(0), need_friend_point(0),
			need_gems(0), once_day_flag(0), gatcha_detail_id(0), gatcha_group_id(0) {}

		uint32_t id, type, priority, need_friend_point, need_gems,
			once_day_flag, gatcha_detail_id, gatcha_group_id;
		std::string name, start_date, end_date, start_hour, end_hour,
			bg_img, btn_img, door_img, caption_msg,
			detail_msg, comment_msg, description, contents_banner_img;

		void Serialize(Json::Value& v) const
		{
			v["7Ffmi96v"] = id;
			v["4N27mkt1"] = name;
			v["S1oz60Hc"] = type;
			v["yu18xScw"] = priority;
			v["qA7M9EjP"] = start_date; // str 2017-10-24 08:00:00
			v["SzV0Nps7"] = end_date; // str 2017-10-31 07:59:59
			v["2HY3jpgu"] = start_hour; // str
			v["v9TR3cDz"] = end_hour; // str
			v["J3stQ7jd"] = need_friend_point;
			v["03UGMHxF"] = need_gems;
			v["4tswNoV9"] = once_day_flag;
			v["1Dg0vUX3"] = bg_img; // str
			v["W9ABuJj2"] = btn_img; // str
			v["uKYf13AH"] = door_img; // str
			v["3sdHQb69"] = caption_msg; // str
			v["W2c9g0Je"] = detail_msg; // str
			v["gVSj32QH"] = comment_msg; // str
			v["TCnm1F4v"] = gatcha_group_id;
			v["qp37xTDh"] = description; // str
			if (gatcha_detail_id > 0)
				v["8Z9CYQDq"] = gatcha_detail_id;
			if (!contents_banner_img.empty())
				v["sA9dDAqB"] = contents_banner_img; // str
		}
	};

	std::vector<Data> Mst;

protected:
	size_t getRespCount() const override { return Mst.size(); }

	void SerializeFields(Json::Value& v, size_t i) const override
	{
		Mst.at(i).Serialize(v);
	}
};
RESPONSE_NS_END
