#pragma once

#include "../GmeRequest.hpp"

RESPONSE_NS_BEGIN
struct VideoAdsSlotgameInfo : public IResponse
{
	const char* getGroupName() const override { return "mebW7mKD"; }

protected:
	void SerializeFields(Json::Value& v, size_t i) const override
	{
		/*
			C38FmiUn -> slotgameinfo:
				zS45RFGb -> ID
				I1Cki7Pb -> slot name
				h1PSnk5t -> reel pos
				b5yeVr61 -> use medal
				jsRoN50z -> slot help url
				TX98PnpE -> slot image
				all the rests are unused...
		*/
		v["C38FmiUn"] = "{\"zS45RFGb\":\"999\",\"I1Cki7Pb\":\"video slots\",\"h1PSnk5t\":\"4,5,6\",\"yu18xScw\":\"1\",\"qA7M9EjP\":\"2014-05-26 06:00:00\",\"SzV0Nps7\":\"2018-12-31 23:59:59\",\"2HY3jpgu\":\"0\",\"v9TR3cDz\":\"0\",\"b5yeVr61\":\"1@0\",\"2iFQ9uhT\":\"999\",\"qp37xTDh\":\"1\",\"jsRoN50z\":\"\\/bf\\/web\\/slots\\/html\\/videoslot.php\",\"TX98PnpE\":\"slot_base.png,slot_btn_on.png,slot_label_start.png,slot_medal_insert.png,slot_medal.png,item_large.png,bravemedal_large.png,sphere_large.png\"}";

		/*
		* iW62Scdg -> SlotgameReelInfoList
		*	PINm2pM5 -> id
		*	Z8eJi4pq -> reel data
		*/
		v["iW62Scdg"] = "[{\"PINm2pM5\":\"4\",\"Z8eJi4pq\":\"14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,1,2,3,4,5,6,7,8,9,10,11,12,13\"},{\"PINm2pM5\":\"5\",\"Z8eJi4pq\":\"4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,1,2,3\"},{\"PINm2pM5\":\"6\",\"Z8eJi4pq\":\"26,27,28,29,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,32\"}]";

		/*
		* rY6j0Jvs -> SlotgamePictureInfoList
		*	sE6tyI9i -> slotpictureid
		*	iQM9dH0F -> slot picture name
		*/
		v["rY6j0Jvs"] = "[{\"sE6tyI9i\":\"14\",\"iQM9dH0F\":\"item.png\"},{\"sE6tyI9i\":\"15\",\"iQM9dH0F\":\"item.png\"},{\"sE6tyI9i\":\"16\",\"iQM9dH0F\":\"item.png\"},{\"sE6tyI9i\":\"17\",\"iQM9dH0F\":\"item.png\"},{\"sE6tyI9i\":\"18\",\"iQM9dH0F\":\"sphere.png\"},{\"sE6tyI9i\":\"19\",\"iQM9dH0F\":\"sphere.png\"},{\"sE6tyI9i\":\"20\",\"iQM9dH0F\":\"e_tome.png\"},{\"sE6tyI9i\":\"21\",\"iQM9dH0F\":\"g_tome.png\"},{\"sE6tyI9i\":\"22\",\"iQM9dH0F\":\"fire_shard.png\"},{\"sE6tyI9i\":\"23\",\"iQM9dH0F\":\"elgif.png\"},{\"sE6tyI9i\":\"24\",\"iQM9dH0F\":\"elgif.png\"},{\"sE6tyI9i\":\"25\",\"iQM9dH0F\":\"elgif.png\"},{\"sE6tyI9i\":\"26\",\"iQM9dH0F\":\"item.png\"},{\"sE6tyI9i\":\"27\",\"iQM9dH0F\":\"s_ticket.png\"},{\"sE6tyI9i\":\"28\",\"iQM9dH0F\":\"s_ticketx2.png\"},{\"sE6tyI9i\":\"29\",\"iQM9dH0F\":\"s_ticketx5.png\"},{\"sE6tyI9i\":\"1\",\"iQM9dH0F\":\"s_flog.png\"},{\"sE6tyI9i\":\"2\",\"iQM9dH0F\":\"b_flog.png\"},{\"sE6tyI9i\":\"3\",\"iQM9dH0F\":\"b_emperor.png\"},{\"sE6tyI9i\":\"4\",\"iQM9dH0F\":\"b_queen.png\"},{\"sE6tyI9i\":\"5\",\"iQM9dH0F\":\"o_frog.png\"},{\"sE6tyI9i\":\"6\",\"iQM9dH0F\":\"pup_unit.png\"},{\"sE6tyI9i\":\"7\",\"iQM9dH0F\":\"allup_unit.png\"},{\"sE6tyI9i\":\"8\",\"iQM9dH0F\":\"c_god.png\"},{\"sE6tyI9i\":\"9\",\"iQM9dH0F\":\"g_god.png\"},{\"sE6tyI9i\":\"10\",\"iQM9dH0F\":\"f_totem.png\"},{\"sE6tyI9i\":\"11\",\"iQM9dH0F\":\"m_mecha.png\"},{\"sE6tyI9i\":\"12\",\"iQM9dH0F\":\"item.png\"},{\"sE6tyI9i\":\"13\",\"iQM9dH0F\":\"item.png\"},{\"sE6tyI9i\":\"32\",\"iQM9dH0F\":\"easter_ticket.png\"}]";
		
		/*
		* tclBMiv2 -> videoadsslotgamestandinfo (USER DATA!)
		*	wRIgGCHh -> ads count
		*	JwBrVzIZ -> max ads count
		*	BrMgnCaT -> current bonus
		*	E9gMeBW0 -> max bount count
		*	qqdr4HlW -> ads bonus flag
		*	er8Ups6U -> next day timer
		*/
		v["tclBMiv2"] = "{\"wRIgGCHh\":\"0\",\"qqdr4HlW\":\"0\",\"E9gMeBW0\":\"42\",\"JwBrVzIZ\":\"10\",\"BrMgnCaT\":\"7\",\"er8Ups6U\":\"0\"}";
	}
};
RESPONSE_NS_END

