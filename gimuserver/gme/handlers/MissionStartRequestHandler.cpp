#include "MissionStartRequestHandler.hpp"
#include "gme/response/SignalKey.hpp"
#include "gme/response/UserUnitInfo.hpp"
#include "gme/response/MissionStartInfo.hpp"
#include "core/System.hpp"

void Handler::MissionStartRequestHandler::Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const
{
	Json::Value res;
	
    Response::MissionStartInfo missionInfo;
    Response::MissionStartInfo::Data d;
    d.userID = user.info.userID;
    d.reinforceUserID = "n9ZMPC0t";
    d.friendPoint = 42640;
    d.missionID = "10";
    d.deckNum = 1;
    missionInfo.Mst.emplace_back(d);
    missionInfo.Serialize(res);

    {
        Json::Value status;
        status["Kn51uR4Y"] = "0h6Q08SL";
        res["6FrKacq7"].append(status);
    }

    {
        Json::Value userState;
        userState["h7eY3sAK"] = "n9ZMPC0t";
        userState["D9wXQI2V"] = "309";
        userState["d96tuT2E"] = "232666";
        userState["YnM14RIP"] = "199";
        userState["0P9X1YHs"] = "196";
        userState["V0yJS7vZ"] = "1650627374";
        userState["f0IY4nj8"] = 540;
        userState["9m5FWR8q"] = "3";
        userState["YS2JG9no"] = "2";
        userState["32HCWt51"] = "1650627005";
        userState["jp9s8IyY"] = 3231;
        userState["ouXxIY63"] = "150";
        userState["Px1X7fcd"] = "620";
        userState["QYP4kId9"] = "383";
        userState["Z0Y4RoD7"] = "1";
        userState["gKNfIZiA"] = 2;
        userState["TwqMChon"] = "-1,-99,-99";
        userState["3u41PhR2"] = "50";
        userState["2rR5s6wn"] = "0";
        userState["5pjoGBC4"] = "200";
        userState["iI7Wj6pM"] = "125";
        userState["J3stQ7jd"] = "42640";
        userState["Najhr8m6"] = "64394391";
        userState["HTVh8a65"] = "99202910";
        userState["03UGMHxF"] = "225";
        userState["bM7RLu5K"] = "La Gimu Trolla PUNTO";
        userState["s2WnRw9N"] = "460,420,430,0,0";
        userState["EfinBo65"] = "7";
        userState["qVBx7g2c"] = "0";
        userState["1RQT92uE"] = "0";
        userState["kW5QuUz7"] = "20220422";
        userState["3w6YDS4z"] = "3";
        userState["lKuj3Ier"] = "";
        userState["JmFn3g9t"] = "0";
        userState["9r3aLmaB"] = "1";
        userState["bya9a67k"] = "2580";
        userState["22rqpZTo"] = "3285";
        userState["KAZmxkgy"] = 0;
        userState["AKP8t3xK"] = 0;
        userState["Nou5bCmm"] = 0;
        userState["s3uU4Lgb"] = 1;
        userState["3a8b9D8i"] = "0";
        userState["7qncTHUJ"] = 0;
        userState["38d7D18b"] = 0;
        userState["D38bda8B"] = 0;
        userState["Qo9doUsp"] = 0;
        userState["d37CaiX1"] = 0;
        userState["92uj7oXB"] = 0;
        res["fEi17cnx"].append(userState);
    }

    {
        Json::Value reinforceUser;
        reinforceUser["h7eY3sAK"] = "n9ZMPC0t";
        reinforceUser["dD64grYH"] = "334";
        reinforceUser["3w6YDS4z"] = "20";
        reinforceUser["d96tuT2E"] = "36216751";
        reinforceUser["pThS5FE3"] = "152762744";
        reinforceUser["mn5Tj3fz"] = "125539575";
        reinforceUser["jG91JRxN"] = "36137127";
        reinforceUser["06phPeqv"] = "35791092";
        reinforceUser["Zq8ej5IN"] = "346035";
        reinforceUser["isRx41jy"] = "147217074";
        reinforceUser["20qd9shE"] = "48090082";
        reinforceUser["Sf95jez7"] = 146280;
        reinforceUser["I29Qgxot"] = "124800";
        reinforceUser["WMC6rNF1"] = "702";
        reinforceUser["Z93pUQhG"] = "2180";
        reinforceUser["Rc6St9h1"] = "45";
        reinforceUser["k5Sjn9Zq"] = "7849";
        reinforceUser["c3Bo97kI"] = "2674";
        reinforceUser["Gt2msFb1"] = "7050";
        reinforceUser["07HgoLtC"] = "1";
        reinforceUser["AEz43gai"] = "0";
        reinforceUser["8CEu9Kcm"] = "0";
        reinforceUser["3DBVLY8H"] = "2887";
        reinforceUser["c4im6B2v"] = 2092;
        reinforceUser["UCN04WxE"] = "1994";
        reinforceUser["ovFJ6Hp0"] = "15045";
        reinforceUser["TW1Mrtp5"] = "502";
        reinforceUser["5NRJQ1LU"] = "824760049";
        reinforceUser["XP06YWdT"] = "569";
        reinforceUser["U8uZLA34"] = "666505";
        reinforceUser["rZQJF5G9"] = "51673";
        reinforceUser["0LwvAF3H"] = "10523";
        reinforceUser["rQ3TAy6I"] = "10116";
        reinforceUser["hoG2ieT5"] = "5461379";
        reinforceUser["6PLsn8xo"] = "2114221";
        reinforceUser["84BC2kXw"] = "3418";
        reinforceUser["5pg7MYCQ"] = "576";
        reinforceUser["mFID53JZ"] = "1124";
        reinforceUser["e6BKoYy9"] = "1814061344";
        res["zI2tJB7R"].append(reinforceUser);
    }

    {
        Json::Value mission;
        mission["ZSf8e1MG"] = "11";
        mission["j28VNcUW"] = "10";
        mission["VETu07N6"] = "1";
        mission["ug9xV4Fz"] = "50";
        mission["5aetPz3C"] = "0";
        mission["mFxqsc7Y"] = "0";
        mission["etM5TCb9"] = "0";
        mission["Qzhp8B40"] = "101301";
        res["pj41dy9g"].append(mission);

        mission["ZSf8e1MG"] = "12";
        mission["j28VNcUW"] = "10";
        mission["VETu07N6"] = "2";
        mission["ug9xV4Fz"] = "50";
        mission["5aetPz3C"] = "0";
        mission["mFxqsc7Y"] = "0";
        mission["etM5TCb9"] = "0";
        mission["Qzhp8B40"] = "101302";
        res["pj41dy9g"].append(mission);

        mission["ZSf8e1MG"] = "14";
        mission["j28VNcUW"] = "10";
        mission["VETu07N6"] = "3";
        mission["ug9xV4Fz"] = "50";
        mission["5aetPz3C"] = "0";
        mission["mFxqsc7Y"] = "0";
        mission["etM5TCb9"] = "0";
        mission["Qzhp8B40"] = "101300";
        res["pj41dy9g"].append(mission);

        mission["ZSf8e1MG"] = "16";
        mission["j28VNcUW"] = "10";
        mission["VETu07N6"] = "4";
        mission["ug9xV4Fz"] = "50";
        mission["5aetPz3C"] = "0";
        mission["mFxqsc7Y"] = "0";
        mission["etM5TCb9"] = "0";
        mission["Qzhp8B40"] = "101302";
        res["pj41dy9g"].append(mission);

        mission["ZSf8e1MG"] = "18";
        mission["j28VNcUW"] = "10";
        mission["VETu07N6"] = "5";
        mission["ug9xV4Fz"] = "100";
        mission["5aetPz3C"] = "0";
        mission["mFxqsc7Y"] = "0";
        mission["etM5TCb9"] = "1";
        mission["Qzhp8B40"] = "101304";
        res["pj41dy9g"].append(mission);
    }

    {
        Json::Value bonus;
        bonus["k9cxD7Ba"] = "58844709";
        bonus["j3g5P4cq"] = "1";
        bonus["nA95Bdj6"] = "0";
        bonus["5Z1LNoyH"] = "0";
        bonus["LE6JkUp7"] = "1|0:25:30030:1:1| @2|0:25:10030:1:1| @3| |1/1/4:25:10000:1@4|1:30:50030:2:2| @5| | ";
        res["Kz7qfSs5"].append(bonus);
    }

    {
        Json::Value missionData;
        missionData["Qzhp8B40"] = "101301";
        missionData["q9I4karx"] = "??????1-2";
        missionData["o49dYfpH"] = "30352";
        missionData["hZtF1s8B"] = "0";
        missionData["3g8PW6x0"] = "180:302";
        missionData["i30R8TAs"] = "7:10000:3,7:10300:3";
        missionData["hw3L0uVj"] = "25:30030:1:0,30:30030:2:0";
        missionData["5wB9SHAV"] = "15,25:500,25:250,25:10,25:10000:1";
        res["75t0sx9z"].append(missionData);

        missionData["Qzhp8B40"] = "101301";
        missionData["q9I4karx"] = "??????1-2";
        missionData["o49dYfpH"] = "40352";
        missionData["hZtF1s8B"] = "1";
        missionData["3g8PW6x0"] = "120:248";
        missionData["i30R8TAs"] = "7:10000:3,7:10300:3";
        missionData["hw3L0uVj"] = "25:40030:1:0,30:40030:2:0";
        missionData["5wB9SHAV"] = "15,25:500,25:250,25:10,25:10000:1";
        res["75t0sx9z"].append(missionData);

        missionData["Qzhp8B40"] = "101302";
        missionData["q9I4karx"] = "??????2-1";
        missionData["o49dYfpH"] = "10352";
        missionData["hZtF1s8B"] = "0";
        missionData["3g8PW6x0"] = "180:302";
        missionData["i30R8TAs"] = "7:10000:3,7:10300:3";
        missionData["hw3L0uVj"] = "25:10030:1:0,30:10030:2:0";
        missionData["5wB9SHAV"] = "15,25:500,25:250,25:10,25:10000:1";
        res["75t0sx9z"].append(missionData);

        missionData["Qzhp8B40"] = "101302";
        missionData["q9I4karx"] = "??????2-1";
        missionData["o49dYfpH"] = "50352";
        missionData["hZtF1s8B"] = "1";
        missionData["3g8PW6x0"] = "120:248";
        missionData["i30R8TAs"] = "7:10000:3,7:10300:3";
        missionData["hw3L0uVj"] = "25:50030:1:0,30:50030:2:0";
        missionData["5wB9SHAV"] = "15,25:500,25:250,25:10,25:10000:1";
        res["75t0sx9z"].append(missionData);

        missionData["Qzhp8B40"] = "101302";
        missionData["q9I4karx"] = "??????2-1";
        missionData["o49dYfpH"] = "30352";
        missionData["hZtF1s8B"] = "2";
        missionData["3g8PW6x0"] = "96:352";
        missionData["i30R8TAs"] = "7:10000:3,7:10300:3";
        missionData["hw3L0uVj"] = "25:30030:1:0,30:30030:2:0";
        missionData["5wB9SHAV"] = "15,25:500,25:250,25:10,25:10000:1";
        res["75t0sx9z"].append(missionData);

        missionData["Qzhp8B40"] = "101300";
        missionData["q9I4karx"] = "??????1-1";
        missionData["o49dYfpH"] = "10352";
        missionData["hZtF1s8B"] = "0";
        missionData["3g8PW6x0"] = "180:302";
        missionData["i30R8TAs"] = "7:10000:3,7:10300:3";
        missionData["hw3L0uVj"] = "25:10030:1:0,30:10030:2:0";
        missionData["5wB9SHAV"] = "15,25:500,25:250,25:10,25:10000:1";
        res["75t0sx9z"].append(missionData);

        missionData["Qzhp8B40"] = "101300";
        missionData["q9I4karx"] = "??????1-1";
        missionData["o49dYfpH"] = "20352";
        missionData["hZtF1s8B"] = "1";
        missionData["3g8PW6x0"] = "120:248";
        missionData["i30R8TAs"] = "7:10000:3,7:10300:3";
        missionData["hw3L0uVj"] = "25:20030:1:0,30:20030:2:0";
        missionData["5wB9SHAV"] = "15,25:500,25:250,25:10,25:10000:1";
        res["75t0sx9z"].append(missionData);

        missionData["Qzhp8B40"] = "101304";
        missionData["q9I4karx"] = "??????boss";
        missionData["o49dYfpH"] = "40401";
        missionData["hZtF1s8B"] = "0";
        missionData["3g8PW6x0"] = "180:302";
        missionData["i30R8TAs"] = "7:10301:5,3:10603:1";
        missionData["hw3L0uVj"] = "0:0:0:0,0:0:0:0";
        missionData["5wB9SHAV"] = "0,25:500,25:250,25:10,25:10301:1";
        res["75t0sx9z"].append(missionData);
    }

    {
        Json::Value enemy;
        enemy["o49dYfpH"] = "30352";
        enemy["Btf93Xs1"] = "????Lv2";
        enemy["3evIn0zZ"] = "????";
        enemy["e7DK0FQT"] = "830";
        enemy["67CApcti"] = "340";
        enemy["q08xLEsy"] = "90";
        enemy["CEeqs63b"] = "0:0:0:0:0:0";
        enemy["iNy0ZU5M"] = "3";
        enemy["eyUo6a8c"] = "24:123:1";
        enemy["6Aou5M9r"] = "30:100:2:1";
        enemy["n9h7p02P"] = "1";
        enemy["Najhr8m6"] = "33";
        enemy["9FN0GAei"] = "5";
        enemy["HTVh8a65"] = "21";
        enemy["vNk2sI4X"] = "5";
        enemy["9pXSKmn1"] = "5";
        enemy["J2hPXGo5"] = "2";
        enemy["mv4o39Uz"] = "1";
        enemy["h2L1YI90"] = "1";
        enemy["XE7Yi5c3"] = "1";
        enemy["D4Y5bWK7"] = "1";
        enemy["oMGC3hW0"] = "1";
        enemy["m4EK7Gt6"] = "1";
        enemy["6fwL59FT"] = "100.00";
        enemy["i74vGUFa"] = "1";
        enemy["F4bQ7r8C"] = "";
        enemy["hjAy9St3"] = "";
        enemy["2Smu5Mtq"] = "";
        enemy["2EF0d6ue"] = "";
        enemy["QqfI9mM4"] = "";
        enemy["CYk84E3W"] = "0,0";
        enemy["3BpHN6VD"] = "";
        enemy["Lkh6gYkT"] = "0,0";
        enemy["pn16CNah"] = "30030";
        enemy["Y6bd4fXp"] = "0,0,0";
        enemy["jm6JSK2D"] = "";
        enemy["qp37xTDh"] = "??87";
        res["U0v5IeJo"].append(enemy);

        enemy["o49dYfpH"] = "40352";
        enemy["Btf93Xs1"] = "????Lv2";
        enemy["3evIn0zZ"] = "????";
        enemy["e7DK0FQT"] = "760";
        enemy["67CApcti"] = "370";
        enemy["q08xLEsy"] = "0";
        enemy["CEeqs63b"] = "0:0:0:0:0:0";
        enemy["iNy0ZU5M"] = "4";
        enemy["eyUo6a8c"] = "24:124:1";
        enemy["6Aou5M9r"] = "30:100:2:1";
        enemy["n9h7p02P"] = "1";
        enemy["Najhr8m6"] = "30";
        enemy["9FN0GAei"] = "5";
        enemy["HTVh8a65"] = "19";
        enemy["vNk2sI4X"] = "5";
        enemy["9pXSKmn1"] = "2";
        enemy["J2hPXGo5"] = "2";
        enemy["mv4o39Uz"] = "1";
        enemy["h2L1YI90"] = "1";
        enemy["XE7Yi5c3"] = "1";
        enemy["D4Y5bWK7"] = "1";
        enemy["oMGC3hW0"] = "1";
        enemy["m4EK7Gt6"] = "1";
        enemy["6fwL59FT"] = "100.00";
        enemy["i74vGUFa"] = "1";
        enemy["F4bQ7r8C"] = "";
        enemy["hjAy9St3"] = "";
        enemy["2Smu5Mtq"] = "";
        enemy["2EF0d6ue"] = "";
        enemy["QqfI9mM4"] = "";
        enemy["CYk84E3W"] = "0,0";
        enemy["3BpHN6VD"] = "";
        enemy["Lkh6gYkT"] = "0,0";
        enemy["pn16CNah"] = "40030";
        enemy["Y6bd4fXp"] = "0,0,0";
        enemy["jm6JSK2D"] = "";
        enemy["qp37xTDh"] = "??120";
        res["U0v5IeJo"].append(enemy);

        enemy["o49dYfpH"] = "10352";
        enemy["Btf93Xs1"] = "????Lv2";
        enemy["3evIn0zZ"] = "????";
        enemy["e7DK0FQT"] = "800";
        enemy["67CApcti"] = "320";
        enemy["q08xLEsy"] = "50";
        enemy["CEeqs63b"] = "0:0:0:0:0:0";
        enemy["iNy0ZU5M"] = "1";
        enemy["eyUo6a8c"] = "24:121:1";
        enemy["6Aou5M9r"] = "30:100:2:1";
        enemy["n9h7p02P"] = "3";
        enemy["Najhr8m6"] = "32";
        enemy["9FN0GAei"] = "5";
        enemy["HTVh8a65"] = "20";
        enemy["vNk2sI4X"] = "5";
        enemy["9pXSKmn1"] = "5";
        enemy["J2hPXGo5"] = "2";
        enemy["mv4o39Uz"] = "1";
        enemy["h2L1YI90"] = "1";
        enemy["XE7Yi5c3"] = "1";
        enemy["D4Y5bWK7"] = "1";
        enemy["oMGC3hW0"] = "1";
        enemy["m4EK7Gt6"] = "1";
        enemy["6fwL59FT"] = "100.00";
        enemy["i74vGUFa"] = "1";
        enemy["F4bQ7r8C"] = "";
        enemy["hjAy9St3"] = "";
        enemy["2Smu5Mtq"] = "";
        enemy["2EF0d6ue"] = "";
        enemy["QqfI9mM4"] = "";
        enemy["CYk84E3W"] = "0,0";
        enemy["3BpHN6VD"] = "";
        enemy["Lkh6gYkT"] = "0,0";
        enemy["pn16CNah"] = "10030";
        enemy["Y6bd4fXp"] = "0,0,0";
        enemy["jm6JSK2D"] = "";
        enemy["qp37xTDh"] = "??21";
        res["U0v5IeJo"].append(enemy);

        enemy["o49dYfpH"] = "50352";
        enemy["Btf93Xs1"] = "????Lv2";
        enemy["3evIn0zZ"] = "????";
        enemy["e7DK0FQT"] = "850";
        enemy["67CApcti"] = "340";
        enemy["q08xLEsy"] = "60";
        enemy["CEeqs63b"] = "0:0:0:0:0:0";
        enemy["iNy0ZU5M"] = "5";
        enemy["eyUo6a8c"] = "24:125:1";
        enemy["6Aou5M9r"] = "30:100:2:1";
        enemy["n9h7p02P"] = "1";
        enemy["Najhr8m6"] = "34";
        enemy["9FN0GAei"] = "5";
        enemy["HTVh8a65"] = "21";
        enemy["vNk2sI4X"] = "5";
        enemy["9pXSKmn1"] = "1";
        enemy["J2hPXGo5"] = "2";
        enemy["mv4o39Uz"] = "1";
        enemy["h2L1YI90"] = "1";
        enemy["XE7Yi5c3"] = "1";
        enemy["D4Y5bWK7"] = "1";
        enemy["oMGC3hW0"] = "1";
        enemy["m4EK7Gt6"] = "1";
        enemy["6fwL59FT"] = "100.00";
        enemy["i74vGUFa"] = "1";
        enemy["F4bQ7r8C"] = "";
        enemy["hjAy9St3"] = "";
        enemy["2Smu5Mtq"] = "";
        enemy["2EF0d6ue"] = "";
        enemy["QqfI9mM4"] = "";
        enemy["CYk84E3W"] = "0,0";
        enemy["3BpHN6VD"] = "";
        enemy["Lkh6gYkT"] = "0,0";
        enemy["pn16CNah"] = "50030";
        enemy["Y6bd4fXp"] = "0,0,0";
        enemy["jm6JSK2D"] = "";
        enemy["qp37xTDh"] = "??153";
        res["U0v5IeJo"].append(enemy);

        enemy["o49dYfpH"] = "20352";
        enemy["Btf93Xs1"] = "????Lv2";
        enemy["3evIn0zZ"] = "????";
        enemy["e7DK0FQT"] = "780";
        enemy["67CApcti"] = "330";
        enemy["q08xLEsy"] = "20";
        enemy["CEeqs63b"] = "0:0:0:0:0:0";
        enemy["iNy0ZU5M"] = "2";
        enemy["eyUo6a8c"] = "24:122:1";
        enemy["6Aou5M9r"] = "30:100:2:1";
        enemy["n9h7p02P"] = "1";
        enemy["Najhr8m6"] = "31";
        enemy["9FN0GAei"] = "5";
        enemy["HTVh8a65"] = "20";
        enemy["vNk2sI4X"] = "5";
        enemy["9pXSKmn1"] = "5";
        enemy["J2hPXGo5"] = "2";
        enemy["mv4o39Uz"] = "1";
        enemy["h2L1YI90"] = "1";
        enemy["XE7Yi5c3"] = "1";
        enemy["D4Y5bWK7"] = "1";
        enemy["oMGC3hW0"] = "1";
        enemy["m4EK7Gt6"] = "1";
        enemy["6fwL59FT"] = "100.00";
        enemy["i74vGUFa"] = "1";
        enemy["F4bQ7r8C"] = "";
        enemy["hjAy9St3"] = "";
        enemy["2Smu5Mtq"] = "";
        enemy["2EF0d6ue"] = "";
        enemy["QqfI9mM4"] = "";
        enemy["CYk84E3W"] = "0,0";
        enemy["3BpHN6VD"] = "";
        enemy["Lkh6gYkT"] = "0,0";
        enemy["pn16CNah"] = "20030";
        enemy["Y6bd4fXp"] = "0,0,0";
        enemy["jm6JSK2D"] = "";
        enemy["qp37xTDh"] = "??54";
        res["U0v5IeJo"].append(enemy);

        enemy["o49dYfpH"] = "40401";
        enemy["Btf93Xs1"] = "???????Lv1";
        enemy["3evIn0zZ"] = "???????";
        enemy["e7DK0FQT"] = "2500";
        enemy["67CApcti"] = "470";
        enemy["q08xLEsy"] = "30";
        enemy["CEeqs63b"] = "0:0:0:0:0:0";
        enemy["iNy0ZU5M"] = "4";
        enemy["eyUo6a8c"] = "36:124:1";
        enemy["6Aou5M9r"] = "42:100:2:1";
        enemy["n9h7p02P"] = "1";
        enemy["Najhr8m6"] = "100";
        enemy["9FN0GAei"] = "5";
        enemy["HTVh8a65"] = "63";
        enemy["vNk2sI4X"] = "5";
        enemy["9pXSKmn1"] = "0";
        enemy["J2hPXGo5"] = "2";
        enemy["mv4o39Uz"] = "1";
        enemy["h2L1YI90"] = "1";
        enemy["XE7Yi5c3"] = "1";
        enemy["D4Y5bWK7"] = "1";
        enemy["oMGC3hW0"] = "1";
        enemy["m4EK7Gt6"] = "1";
        enemy["6fwL59FT"] = "100.00";
        enemy["i74vGUFa"] = "1";
        enemy["F4bQ7r8C"] = "";
        enemy["hjAy9St3"] = "";
        enemy["2Smu5Mtq"] = "";
        enemy["2EF0d6ue"] = "";
        enemy["QqfI9mM4"] = "";
        enemy["CYk84E3W"] = "0,0";
        enemy["3BpHN6VD"] = "";
        enemy["Lkh6gYkT"] = "0,0";
        enemy["pn16CNah"] = "40031";
        enemy["Y6bd4fXp"] = "0,0,0";
        enemy["jm6JSK2D"] = "";
        enemy["qp37xTDh"] = "??121";
        res["U0v5IeJo"].append(enemy);
    }

    res["8hoyIF9Q"] = Json::arrayValue;
    res["VZwB7f3j"] = Json::arrayValue;

    {
        Json::Value status;
        status["Kn51uR4Y"] = "0";
        res["nAligJSQ"].append(status);
    }

    {
        Json::Value clientInfo;
        clientInfo["h7eY3sAK"] = "n9ZMPC0t";
        clientInfo["B5JQyV8j"] = "Arves100";
        clientInfo["iN7buP0j"] = "WAS-LX1A_android8.0.0";
        clientInfo["Ma5GnU0H"] = "4e457983-74b0-4ea7-9a98-1c5890dfc836";
        res["IKqx1Cn9"].append(clientInfo);
    }

    {
        Json::Value announcement;
        announcement["xJNom6i0"] = "3876";
        announcement["jsRoN50z"] = "http://ios21900.bfww.gumi.sg//news.gumi.sg/bravefrontier/news/files/html/2022-03/Closure_Announcement_033022_1648608188.html";
        res["Pj6zDW3m"] = announcement;
    }

    std::cout << "MissionStartHandler response: " << res.toStyledString() << std::endl;

	cb(newGmeOkResponse(GetGroupId(), GetAesKey(), res));
}
