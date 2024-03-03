#include "ControlCenterEnterHandler.hpp"
#include "gme/response/SlotGameInfo_Resp.hpp"

void Handler::ControlCenterEnterHandler::Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const
{
	Json::Value res;
	user.teamInfo.Serialize(res);
	Response::SlotGameInfoR r;
	r.info.id = 1;
	r.info.name = "Brave Slots";
	r.info.reelPos = "1,2,3";
	r.info.useMedal = 1;
	r.info.slotHelpUrl = "/bf/web/slots/html/index.php";
	r.info.slotImage = "slot_base.png,slot_btn_on.png,slot_label_start.png,slot_medal_insert.png,slot_medal.png,item_large.png,bravemedal_large.png,sphere_large.png";
	Response::SlotGamePictureInfo::Data d;
	d.id = 81;
	d.pictureName = "j_god.png";
	r.pictures.Mst.emplace_back(d);
	d.id = 1;
	d.pictureName = "s_flog.png";
	r.pictures.Mst.emplace_back(d);
	d.id = 7;
	d.pictureName = "allup_unit.png";
	r.pictures.Mst.emplace_back(d);
	d.id = 2;
	d.pictureName = "b_flog.png";
	r.pictures.Mst.emplace_back(d);
	d.id = 8;
	d.pictureName = "c_god.png";
	r.pictures.Mst.emplace_back(d);
	d.id = 6;
	d.pictureName = "pup_unit.png";
	r.pictures.Mst.emplace_back(d);
	d.id = 3;
	d.pictureName = "b_emperor.png";
	r.pictures.Mst.emplace_back(d);
	d.id = 18;
	d.pictureName = "sphere.png";
	r.pictures.Mst.emplace_back(d);
	d.id = 9;
	d.pictureName = "g_god.png";
	r.pictures.Mst.emplace_back(d);
	d.id = 80;
	d.pictureName = "m_god.png";
	r.pictures.Mst.emplace_back(d);
	d.id = 13;
	d.pictureName = "item.png";
	r.pictures.Mst.emplace_back(d);
	d.id = 12;
	d.pictureName = "item.png";
	r.pictures.Mst.emplace_back(d);
	d.id = 15;
	d.pictureName = "item.png";
	r.pictures.Mst.emplace_back(d);
	d.id = 82;
	d.pictureName = "medal.png";
	r.pictures.Mst.emplace_back(d);
	r.Serialize(res);

	cb(newGmeOkResponse(GetGroupId(), GetAesKey(), res));
}
