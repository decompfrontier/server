#pragma once

#include "PacketInterface.hpp"

namespace db
{

/*!
* Database mapping for login/user identity fields stored in user_info.
*/
template <>
inline PacketInterfaceFor<::LoginInfoResp>::Fields
PacketInterfaceFor<::LoginInfoResp>::fields()
{
	return {
		field<&::LoginInfoResp::user_id>("id", {
			.read = true,
			.insert = true,
		}),
		field<&::LoginInfoResp::gumi_live_userid>("gumi_user_id", {
			.read = true,
			.insert = true,
		}),
		field<&::LoginInfoResp::handle_name>("username", {
			.read = true,
			.update = true,
			.insert = true,
		}),
		field<&::LoginInfoResp::tutorial_status>("tutorial_status", {
			.read = true,
			.update = true,
			.insert = true,
		}),
	};
}

/*!
* Database mapping for team/account summary fields stored in user_info.
*/
template <>
inline PacketInterfaceFor<::UserTeamInfo>::Fields
PacketInterfaceFor<::UserTeamInfo>::fields()
{
	return {
		field<&::UserTeamInfo::user_id>("id", {
			.read = true,
			.insert = true,
		}),
		field<&::UserTeamInfo::level>("level", {
			.read = true,
			.update = true,
			.insert = true,
		}),
		field<&::UserTeamInfo::exp>("exp", {
			.read = true,
			.update = true,
			.insert = true,
		}),
		field<&::UserTeamInfo::energy>("energy", {
			.read = true,
			.update = true,
			.insert = true,
		}),
		field<&::UserTeamInfo::max_unit_count>("max_unit_count", {
			.read = true,
			.update = true,
			.insert = true,
		}),
		field<&::UserTeamInfo::warehouse_count>("max_warehouse_count", {
			.read = true,
			.update = true,
			.insert = true,
		}),
		field<&::UserTeamInfo::active_deck>("active_deck", {
			.read = true,
			.update = true,
			.insert = true,
		}),
		field<&::UserTeamInfo::zel>("zel", {
			.read = true,
			.update = true,
			.insert = true,
		}),
		field<&::UserTeamInfo::karma>("karma", {
			.read = true,
			.update = true,
			.insert = true,
		}),
		// The client's gem HUD reads brave_coin (03UGMHxF), so dev wires the gems
		// column here on purpose (the "BraveCoin" readParam setter name is a red
		// herring for this client). Do NOT point this at a real brave_coin column
		// — that zeroed the HUD and broke gems. Kept mapped to `gems` with
		// paid_gems/free_gems so every gem field the client might read shows gems.
		field<&::UserTeamInfo::brave_coin>("gems", {
			.read = true,
			.update = true,
			.insert = true,
		}),
		field<&::UserTeamInfo::friend_point>("friend_points", {
			.read = true,
			.update = true,
			.insert = true,
		}),
		field<&::UserTeamInfo::summon_ticket>("summon_tickets", {
			.read = true,
		}),
		field<&::UserTeamInfo::rainbow_coin>("rainbow_coins", {
			.read = true,
		}),
		field<&::UserTeamInfo::colosseum_ticket>("colosseum_tickets", {
			.read = true,
		}),
		field<&::UserTeamInfo::brave_points_total>("total_brave_points", {
			.read = true,
		}),
		field<&::UserTeamInfo::current_brave_points>("avail_brave_points", {
			.read = true,
		}),
		field<&::UserTeamInfo::want_gift>("want_gift", {
			.read = true,
		}),
		// Send the single `gems` column to BOTH gem fields. The HUD reads FREE
		// gems (92uj7oXB) — it was being sent as 0, which is why the balance
		// never updated (while zel, whose field IS read, did). The summon/paid
		// path reads PAID gems (d37CaiX1) — the user could summon earlier when
		// only paid was populated, so keep it set too. A captured production
		// team_info has both populated (free=10000, paid=20000); mirroring one
		// column to both keeps display + summon working without double-counting
		// (the HUD shows free, not the sum).
		field<&::UserTeamInfo::free_gems>("gems", {
			.read = true,
		}),
		field<&::UserTeamInfo::paid_gems>("gems", {
			.read = true,
		}),
	};
}

/*!
* Database mapping for player-owned units stored in user_units.
*/
template <>
inline PacketInterfaceFor<::UserUnitInfo>::Fields
PacketInterfaceFor<::UserUnitInfo>::fields()
{
	return {
		field<&::UserUnitInfo::user_unit_id>("user_unit_id", {
			.read = true,
		}),
		field<&::UserUnitInfo::received_order>("user_unit_id", {
			.read = true,
		}),
		field<&::UserUnitInfo::user_id>("user_id", {
			.read = true,
			.insert = true,
		}),
		field<&::UserUnitInfo::unit_id>("unit_id", {
			.read = true,
			.insert = true,
		}),
		field<&::UserUnitInfo::unit_type_id>("unit_type_id", {
			.read = true,
			.update = true,
			.insert = true,
		}),
		field<&::UserUnitInfo::unit_lvl>("unit_lvl", {
			.read = true,
			.update = true,
			.insert = true,
		}),
		field<&::UserUnitInfo::base_hp>("base_hp", {
			.read = true,
			.update = true,
			.insert = true,
		}),
		field<&::UserUnitInfo::base_atk>("base_atk", {
			.read = true,
			.update = true,
			.insert = true,
		}),
		field<&::UserUnitInfo::base_def>("base_def", {
			.read = true,
			.update = true,
			.insert = true,
		}),
		// base_rec is INTEGER NOT NULL with NO default, so the INSERT must keep
		// providing it (INSERT OR IGNORE silently drops the row otherwise). Read
		// stays on the canonical column; addUserUnit writes it.
		field<&::UserUnitInfo::base_rec>("base_rec", {
			.read = true,
			.update = true,
			.insert = true,
		}),
		field<&::UserUnitInfo::ext_hp>("ext_hp", {
			.read = true,
			.update = true,
			.insert = true,
		}),
		field<&::UserUnitInfo::ext_atk>("ext_atk", {
			.read = true,
			.update = true,
			.insert = true,
		}),
		field<&::UserUnitInfo::ext_def>("ext_def", {
			.read = true,
			.update = true,
			.insert = true,
		}),
		field<&::UserUnitInfo::ext_rec>("ext_rec", {
			.read = true,
			.update = true,
			.insert = true,
		}),
		field<&::UserUnitInfo::bb_id>("bb_id", {
			.read = true,
			.update = true,
			.insert = true,
		}),
		field<&::UserUnitInfo::bb_lvl>("bb_lvl", {
			.read = true,
			.update = true,
			.insert = true,
		}),
		field<&::UserUnitInfo::sbb_id>("sbb_id", {
			.read = true,
			.update = true,
			.insert = true,
		}),
		field<&::UserUnitInfo::sbb_lvl>("sbb_lvl", {
			.read = true,
			.update = true,
			.insert = true,
		}),
		// Extras the read/display path was missing — mapped to the quests mirror
		// columns the unit handlers write, so units keep full stats/level/element
		// across a UserInfo reload (previously lost to column defaults).
		field<&::UserUnitInfo::exp>("exp", { .read = true, .update = true, .insert = true, }),
		field<&::UserUnitInfo::total_exp>("total_exp", { .read = true, .update = true, .insert = true, }),
		field<&::UserUnitInfo::add_hp>("add_hp", { .read = true, .update = true, .insert = true, }),
		field<&::UserUnitInfo::add_atk>("add_atk", { .read = true, .update = true, .insert = true, }),
		field<&::UserUnitInfo::add_def>("add_def", { .read = true, .update = true, .insert = true, }),
		field<&::UserUnitInfo::add_rec>("add_rec", { .read = true, .update = true, .insert = true, }),
		field<&::UserUnitInfo::limit_over_hp>("limit_over_hp", { .read = true, .update = true, .insert = true, }),
		field<&::UserUnitInfo::limit_over_atk>("limit_over_atk", { .read = true, .update = true, .insert = true, }),
		field<&::UserUnitInfo::limit_over_def>("limit_over_def", { .read = true, .update = true, .insert = true, }),
		field<&::UserUnitInfo::limit_over_rec>("limit_over_rec", { .read = true, .update = true, .insert = true, }),
		field<&::UserUnitInfo::element>("element", { .read = true, .update = true, .insert = true, }),
		field<&::UserUnitInfo::equipitem_id>("eqip_item_id", { .read = true, .update = true, .insert = true, }),
		field<&::UserUnitInfo::equipitem_frame_id>("eqip_item_frame_id", { .read = true, .update = true, .insert = true, }),
		field<&::UserUnitInfo::equipitem_id2>("eqip_item_id2", { .read = true, .update = true, .insert = true, }),
		field<&::UserUnitInfo::equipitem_frame_id2>("eqip_item_frame_id2", { .read = true, .update = true, .insert = true, }),
		field<&::UserUnitInfo::is_new>("new", {
			.read = true,
			.update = true,
			.insert = true,
		}),
	};
}

/*!
* Database mapping for discovered unit species stored in user_unit_dictionary.
*/
template <>
inline PacketInterfaceFor<::UserUnitDictionary>::Fields
PacketInterfaceFor<::UserUnitDictionary>::fields()
{
	return {
		field<&::UserUnitDictionary::user_id>("user_id", {
			.read = true,
			.insert = true,
		}),
		field<&::UserUnitDictionary::unit_id>("unit_id", {
			.read = true,
			.insert = true,
		}),
		field<&::UserUnitDictionary::img_type_flag>("img_type_flag", {
			.read = true,
			.update = true,
			.insert = true,
		}),
	};
}

/*!
* Database mapping for owned item stacks stored in user_items.
*/
template <>
inline PacketInterfaceFor<::UserWarehouseInfo>::Fields
PacketInterfaceFor<::UserWarehouseInfo>::fields()
{
	return {
		field<&::UserWarehouseInfo::instance_id>("instance_id", {
			.read = true,
		}),
		field<&::UserWarehouseInfo::item_id>("item_id", {
			.read = true,
			.insert = true,
		}),
		field<&::UserWarehouseInfo::item_num>("item_num", {
			.read = true,
			.update = true,
			.insert = true,
		}),
		field<&::UserWarehouseInfo::favorite_flg>("favorite_flg", {
			.read = true,
			.update = true,
		}),
		field<&::UserWarehouseInfo::disp_order>("disp_order", {
			.read = true,
			.update = true,
		}),
	};
}

/*!
* Database mapping for party deck slots stored in user_decks.
*/
template <>
inline PacketInterfaceFor<::UserPartyDeckInfo>::Fields
PacketInterfaceFor<::UserPartyDeckInfo>::fields()
{
	return {
		field<&::UserPartyDeckInfo::user_unit_id>("user_unit_id", {
			.read = true,
			.update = true,
			.insert = true,
		}),
		field<&::UserPartyDeckInfo::deck_type>("deck_type", {
			.read = true,
			.insert = true,
		}),
		field<&::UserPartyDeckInfo::deck_num>("deck_num", {
			.read = true,
			.insert = true,
		}),
		field<&::UserPartyDeckInfo::member_type>("member_type", {
			.read = true,
			.update = true,
			.insert = true,
		}),
		field<&::UserPartyDeckInfo::disp_order>("disp_order", {
			.read = true,
			.insert = true,
		}),
	};
}

} // namespace db
