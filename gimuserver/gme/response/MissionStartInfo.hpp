#pragma once

#include "../GmeRequest.hpp"

RESPONSE_NS_BEGIN
struct MissionStartInfo : public IResponse
{
    struct Data
    {
        std::string userID;
        std::string reinforceUserID;
        uint32_t friendPoint;
        std::string missionID;
        std::string reinforceInfo; // "1VagK32J" field, not present in the provided response
        uint32_t deckNum;
        uint32_t bonusType;
        float bonusRate;

        explicit Data() : friendPoint(0), deckNum(0), bonusType(0), bonusRate(0.0f)
        {
        }

        void Serialize(Json::Value& v) const
        {
            v["h7eY3sAK"] = reinforceUserID; // str
            v["J3stQ7jd"] = std::to_string(friendPoint); // str
            v["j28VNcUW"] = missionID; // str
            if (!reinforceInfo.empty()) {
                v["1VagK32J"] = reinforceInfo; // str
            }
            v["Z0Y4RoD7"] = std::to_string(deckNum); // str
            if (bonusType != 0) {
                v["nA95Bdj6"] = std::to_string(bonusType); // str
            }
            if (bonusRate != 0.0f) {
                v["5Z1LNoyH"] = std::to_string(bonusRate); // str
            }
        }
    };

    const char* getGroupName() const override { return "fEi17cnx"; }

    std::vector<Data> Mst;

protected:
    size_t getRespCount() const override { return Mst.size(); }

    void SerializeFields(Json::Value& v, size_t i) const override
    {
        Mst.at(i).Serialize(v);
    }
};
RESPONSE_NS_END