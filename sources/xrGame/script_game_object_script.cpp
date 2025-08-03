////////////////////////////////////////////////////////////////////////////
//	Module 		: script_game_object_script.cpp
//	Created 	: 25.09.2003
//  Modified 	: 29.06.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay Script game object script export
////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include "script_game_object.h"
#include "GameObject_space.h"
#include "script_ini_file.h"
#include "sight_manager_space.h"

using namespace luabind;

extern class_<CScriptGameObject> &script_register_game_object1(class_<CScriptGameObject> &);
extern class_<CScriptGameObject> &script_register_game_object2(class_<CScriptGameObject> &);
extern class_<CScriptGameObject> &script_register_game_object_trader(class_<CScriptGameObject> &);

#pragma optimize("s",on)
void CScriptGameObject::script_register(lua_State *L)
{
	class_<CScriptGameObject>	instance("game_object");

	module(L)
	[
		class_<CSightParams>("CSightParams")
			.enum_("bla-bla")
			[
				value("eSightTypeCurrentDirection", s32(SightManager::eSightTypeCurrentDirection)),
				value("eSightTypePathDirection", s32(SightManager::eSightTypePathDirection	)),
				value("eSightTypeDirection", s32(SightManager::eSightTypeDirection		)),
				value("eSightTypePosition", s32(SightManager::eSightTypePosition		)),
				value("eSightTypeObject", s32(SightManager::eSightTypeObject			)),
				value("eSightTypeCover", s32(SightManager::eSightTypeCover			)),
				value("eSightTypeSearch", s32(SightManager::eSightTypeSearch			)),
				value("eSightTypeLookOver", s32(SightManager::eSightTypeLookOver		)),
				value("eSightTypeCoverLookOver", s32(SightManager::eSightTypeCoverLookOver	)),
				value("eSightTypeFireObject", s32(SightManager::eSightTypeFireObject		)),
				value("eSightTypeFirePosition", s32(SightManager::eSightTypeFirePosition	)),
				value("eSightTypeDummy", s32(SightManager::eSightTypeDummy			))
			]
			.def(							constructor<>())
			.def_readonly("m_object",		&CSightParams::m_object)
			.def_readonly("m_vector",		&CSightParams::m_vector)
			.def_readonly("m_sight_type",	&CSightParams::m_sight_type),
		
		script_register_game_object2(
			script_register_game_object1(
				script_register_game_object_trader(instance)
			)
		),

		class_<enum_exporter<GameObject::ECallbackType> >("callback")
			.enum_("callback_types")
			[
				value("trade_start", s32(GameObject::eTradeStart)),
				value("trade_stop", s32(GameObject::eTradeStop)),
				value("trade_sell_buy_item", s32(GameObject::eTradeSellBuyItem)),
				value("trade_perform_operation", s32(GameObject::eTradePerformTradeOperation)),
				value("trader_global_anim_request", s32(GameObject::eTraderGlobalAnimationRequest)),
				value("trader_head_anim_request", s32(GameObject::eTraderHeadAnimationRequest)),
				value("trader_sound_end", s32(GameObject::eTraderSoundEnd)),
				value("zone_enter", s32(GameObject::eZoneEnter)),
				value("zone_exit", s32(GameObject::eZoneExit)),
				value("level_border_exit", s32(GameObject::eExitLevelBorder)),
				value("level_border_enter", s32(GameObject::eEnterLevelBorder)),
				value("death", s32(GameObject::eDeath)),
				value("patrol_path_in_point", s32(GameObject::ePatrolPathInPoint)),
				value("inventory_pda", s32(GameObject::eInventoryPda)),
				value("inventory_info", s32(GameObject::eInventoryInfo)),
				value("article_info", s32(GameObject::eArticleInfo)),
				value("use_object", s32(GameObject::eUseObject)),
				value("hit", s32(GameObject::eHit)),
				value("sound", s32(GameObject::eSound)),
				value("action_movement", s32(GameObject::eActionTypeMovement)),
				value("action_watch", s32(GameObject::eActionTypeWatch)),
				value("action_animation", s32(GameObject::eActionTypeAnimation)),
				value("action_sound", s32(GameObject::eActionTypeSound)),
				value("action_particle", s32(GameObject::eActionTypeParticle)),
				value("action_object", s32(GameObject::eActionTypeObject)),
				value("actor_sleep", s32(GameObject::eActorSleep)),
				value("helicopter_on_point", s32(GameObject::eHelicopterOnPoint)),
				value("helicopter_on_hit", s32(GameObject::eHelicopterOnHit)),
				value("on_item_take", s32(GameObject::eOnItemTake)),
				value("on_item_drop", s32(GameObject::eOnItemDrop)),
				value("script_animation", s32(GameObject::eScriptAnimation)),
				value("task_state", s32(GameObject::eTaskStateChange)),
				value("take_item_from_box", s32(GameObject::eInvBoxItemTake)),
				value("map_location_added", s32(GameObject::eMapLocationAdded))
			],

		def("buy_condition",				(void (*)(CScriptIniFile*, pcstr))(&::buy_condition)),
		def("buy_condition",				(void (*)(f32, f32))(&::buy_condition)),
		def("sell_condition",				(void (*)(CScriptIniFile*, pcstr))(&::sell_condition)),
		def("sell_condition",				(void (*)(f32, f32))(&::sell_condition)),
		def("show_condition",				&::show_condition)
	];
}