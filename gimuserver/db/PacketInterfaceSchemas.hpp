#pragma once

#include "PacketInterface.hpp"

namespace db
{

/*!
* Database mapping for login/user identity fields stored in userinfo.
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
* Database mapping for team/account summary fields stored in userinfo.
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
		field<&::UserUnitInfo::user_id>("user_id", {
			.read = true,
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
