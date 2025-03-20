#include "GatchaActionHandler.hpp"
#include "gme/response/SignalKey.hpp"
#include "gme/response/UserUnitInfo.hpp"
#include "core/System.hpp"

void Handler::GatchaActionHandler::Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const
{
    Json::Value res;

    /*
        req = {
            "1IR86sAv" : [
                {
                    "324b023k" : "1",           if we used summoning ticket ?
                    "7Ffmi96v" : "17160",       gatcha id
                    "a329kbl8" : "1"            idx of gatcha request ?
                }
            ],
            ...
        }
    */

	// For now, hard-code the response to return unit
    Response::UserUnitInfo unitInfo;

    // Only add to user unit collection
    unitInfo.overwrite = false;
    if (unitInfo.Mst.empty()) {
        Response::UserUnitInfo::Data d;
        d.userID = user.info.userID;
        d.userUnitID = 9999;                   // ensure that this is unique
        d.unitID = 50256;           
        d.unitTypeID = 1;
        d.element = "light";
        d.unitLv = 1;
        d.newFlg = 1;
        d.receiveDate = 100;
        d.FeBP = 100;
        d.FeMaxUsableBP = 200;
        d.baseHp = 5000;
        d.baseAtk = 1000;
        d.baseDef = 1000;
        d.baseHeal = 1000;
        d.addHp = 100;
        d.addAtk = 100;
        d.addDef = 100;
        d.addHeal = 100;
        d.extHp = 100;
        d.extAtk = 100;
        d.extDef = 100;
        d.extHeal = 100;
        d.limitOverHP = 200;
        d.limitOverAtk = 200;
        d.limitOverDef = 200;
        d.limitOverHeal = 200;
        d.exp = 1;
        d.totalExp = 100;
        unitInfo.Mst.emplace_back(d);
    }
    unitInfo.Serialize(res);

    // Hard-coded response for OpeUserUnitResponse
    {
		Json::Value d;
		d["edy7fq3L"] = "9999";    // userUnitID to open -> should match the one that was just added
		d["u0vkt9yH"] = 13762;     // Which summon gate picture to use
        //d["g30VnzQh"] = ?;       // In IDA this is parsed as GatchaChangeRateInfo, so maybe its for specific banners?
		res["Km35HAXv"] = d;
    }

	cb(newGmeOkResponse(GetGroupId(), GetAesKey(), res));
}
