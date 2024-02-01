#include "MstInfo.hpp"
#include <json/json.h>
#include <fstream>

bool MstInfo::LoadTableFromJson(const std::string& path)
{
	std::ifstream ifs;
	ifs.open(path);

	if (!ifs.is_open())
		return false;

	std::vector<MstInfo::Table> tables;

	try
	{
		Json::CharReaderBuilder rb;
		JSONCPP_STRING jss;
		Json::Value root;
		if (!Json::parseFromStream(rb, ifs, &root, &jss))
			return false;

		const auto& msts = root["MSTs"];

		for (const auto& v : msts)
		{
			MstInfo::Table tbl;
			tbl.Name = v["Name"].asCString();
			tbl.Description = v["Description"].asCString();
			tbl.Version = v["Version"].asInt();
			tbl.Unknown = v["Unknown"].asInt();
			tbl.SubVersion = v["SubVersion"].asInt();
			tables.emplace_back(tbl);
		}
	}
	catch (const std::exception& ex)
	{
		return false;
	}

	// now serialize this into a BF json to save time

	Json::Value mstListJson;
	Json::Value::ArrayIndex i = 0;
	for (const auto& v : tables)
	{
		Json::Value g;
		g["moWQ30GH"] = v.Name;
		g["e3QNsuZ8"] = ""; //v.Description;
		g["d2RFtP8T"] = v.Version;
		g["H6k1LIxC"] = v.Unknown;
		g["5kbnkTp0"] = v.SubVersion;

		mstListJson.insert(i, g);
		i++;
	}

	m_convMst = mstListJson;

	return true;
}

void MstInfo::GetMstData(Json::Value& v) const
{
	v["KeC10fuL"] = m_convMst;
}
