#include "GachaActionRequestHandler.hpp"
#include "gme/response/SignalKey.hpp"
#include "gme/response/UserUnitInfo.hpp"
#include "core/System.hpp"

void Handler::GachaActionRequestHandler::Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const
{
	Json::Value res;
	
    //TODO: Tuck away into a response class

    Response::UserUnitInfo unitInfo;
    unitInfo.overwrite = false;
    if (unitInfo.Mst.empty()) {
        Response::UserUnitInfo::Data d;
        d.userID = user.info.userID;
        d.userUnitID = 9999; // TODO: Intigrate these values to inject data into the SQL for longterm storage.
        d.unitID = 10011; // TODO: This unit is hard-coded and needs to be randomized to the rates of a given gatcha table.
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

    {
        Json::Value d;
        d["edy7fq3L"] = "9999"; // Unique String for Unit
        d["u0vkt9yH"] = 13762; //Flag for Gate Anim
        res["Km35HAXv"] = d;
    }

	cb(newGmeOkResponse(GetGroupId(), GetAesKey(), res));
}
