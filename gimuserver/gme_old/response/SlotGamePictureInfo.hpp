#pragma once

#include "../GmeRequest.hpp"

RESPONSE_NS_BEGIN
struct SlotGamePictureInfo : public IResponse
{
	struct Data
	{
		uint32_t id;
		std::string pictureName;

		explicit Data() : id(0) {}

		void Serialize(Json::Value& v) const
		{
			v["sE6tyI9i"] = std::to_string(id);
			v["iQM9dH0F"] = pictureName;
		}
	};

	const char* getGroupName() const override { return nullptr; }

	std::vector<Data> Mst;

protected:
	size_t getRespCount() const override { return Mst.size(); }

	void SerializeFields(Json::Value& v, size_t i) const override
	{
		Mst.at(i).Serialize(v);
	}
};
RESPONSE_NS_END
