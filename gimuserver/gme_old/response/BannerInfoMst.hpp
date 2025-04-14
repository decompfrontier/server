#pragma once

#include "../GmeRequest.hpp"

RESPONSE_NS_BEGIN
struct BannerInfoMst : public IResponse
{
	const char* getGroupName() const override { return "Pk5F1vhx"; }

	struct Data
	{
		explicit Data() : id(1), readCount(0), dispOrder(0) {}

		uint32_t id, readCount, dispOrder;
		std::string name, url, image, param, pageType,
			targetOS;

		void Serialize(Json::Value& v) const
		{
			v["oL71Fz3a"] = std::to_string(id);
			v["NyYKc1A5"] = name;
			v["aL70hVYQ"] = targetOS;
			v["XuJL4pc5"] = std::to_string(dispOrder); // int
			v["jsRoN50z"] = url;
			v["1gDkL6XR"] = image;
			v["t5R47iwj"] = param;
			v["LM34kfVC"] = pageType;
			v["d36D1g8T"] = readCount; // int
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
