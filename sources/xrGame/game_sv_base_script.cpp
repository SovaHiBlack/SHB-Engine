////////////////////////////////////////////////////////////////////////////
//	Module 		: game_sv_base_script.cpp
//	Created 	: 28.06.2004
//  Modified 	: 28.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Base server game script export
////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include "game_sv_base.h"
#include "xrMessages.h"
#include "ui/UIInventoryUtilities.h"//
#include "xr_time.h"
#include "..\xrNetServer\net_utils.h"
#include "UI/UIGameTutorial.h"
#include "StringTable.h"
#include "object_broker.h"

using namespace luabind;


CUISequencer* g_tutorial = NULL;
CUISequencer* g_tutorial2 = NULL;

void start_tutorial(pcstr name)
{
	if(g_tutorial){
		VERIFY				(!g_tutorial2);
		g_tutorial2			= g_tutorial;
	}
		
	g_tutorial							= xr_new<CUISequencer>();
	g_tutorial->Start					(name);
	if (g_tutorial2)
	{
		g_tutorial->m_pStoredInputReceiver = g_tutorial2->m_pStoredInputReceiver;
	}
}

pcstr translate_string(pcstr str)
{
	return *CStringTable().translate(str);
}

bool has_active_tutotial()
{
	return (g_tutorial!=NULL);
}

#pragma optimize("s",on)
void game_sv_GameState::script_register(lua_State *L)
{

	module(L,"game")
	[
	class_< xrTime >("CTime")
		.enum_("date_format")
		[
			value("DateToDay", s32(InventoryUtilities::edpDateToDay)),
			value("DateToMonth", s32(InventoryUtilities::edpDateToMonth)),
			value("DateToYear", s32(InventoryUtilities::edpDateToYear))
		]
		.enum_("time_format")
		[
			value("TimeToHours", s32(InventoryUtilities::etpTimeToHours)),
			value("TimeToMinutes", s32(InventoryUtilities::etpTimeToMinutes)),
			value("TimeToSeconds", s32(InventoryUtilities::etpTimeToSeconds)),
			value("TimeToMilisecs", s32(InventoryUtilities::etpTimeToMilisecs))
		]
		.def(						constructor<>()				)
		.def(						constructor<const xrTime&>())
		.def(const_self <			xrTime()					)
		.def(const_self <=			xrTime()					)
		.def(const_self >			xrTime()					)
		.def(const_self >=			xrTime()					)
		.def(const_self ==			xrTime()					)
		.def(self +					xrTime()					)
		.def(self -					xrTime()					)

		.def("diffSec"				,&xrTime::diffSec_script)
		.def("add"					,&xrTime::add_script)
		.def("sub"					,&xrTime::sub_script)

		.def("setHMS"				,&xrTime::setHMS)
		.def("setHMSms"				,&xrTime::setHMSms)
		.def("set"					,&xrTime::set)
		.def("get"					,&xrTime::get, out_value(_2) + out_value(_3) + out_value(_4) + out_value(_5) + out_value(_6) + out_value(_7) + out_value(_8))
		.def("dateToString"			,&xrTime::dateToString)
		.def("timeToString"			,&xrTime::timeToString),
		// declarations
		def("time",					get_time),
		def("get_game_time",		get_time_struct),
//		def("get_surge_time",	Game::get_surge_time),
//		def("get_object_by_name",Game::get_object_by_name),
	
	class_< game_sv_GameState, game_GameState >("game_sv_GameState")

	.def("get_eid",				&game_sv_GameState::get_eid)
	.def("get_id",				&game_sv_GameState::get_id)
	.def("get_it",				&game_sv_GameState::get_it)
	.def("get_it_2_id",			&game_sv_GameState::get_it_2_id)
	.def("get_name_it",			&game_sv_GameState::get_name_it)
	.def("get_name_id",			&game_sv_GameState::get_name_id)
	.def("get_player_name_id",	&game_sv_GameState::get_player_name_id)
	
	.def("get_players_count",	&game_sv_GameState::get_players_count)
	.def("get_id_2_eid",		&game_sv_GameState::get_id_2_eid)

	.def("get_option_i",		&game_sv_GameState::get_option_i)
	.def("get_option_s",		&game_sv_GameState::get_option_s)
	.def("u_EventSend",			&game_sv_GameState::u_EventSend)

	.def("GenerateGameMessage",	&game_sv_GameState::GenerateGameMessage)
	.def("getRP",				&game_sv_GameState::getRP)
	.def("getRPcount",			&game_sv_GameState::getRPcount),

	def("start_tutorial",		&start_tutorial),
	def("has_active_tutorial",	&has_active_tutotial),
	def("translate_string",		&translate_string)

	];
	
	module(L)
	[
	class_<enum_exporter<EGamePlayerFlags> >("game_player_flags")
		.enum_("flags")
		[
			value("GAME_PLAYER_FLAG_LOCAL", s32(GAME_PLAYER_FLAG_LOCAL)),
			value("GAME_PLAYER_FLAG_READY", s32(GAME_PLAYER_FLAG_READY)),
			value("GAME_PLAYER_FLAG_VERY_VERY_DEAD", s32(GAME_PLAYER_FLAG_VERY_VERY_DEAD)),
			value("GAME_PLAYER_FLAG_SPECTATOR", s32(GAME_PLAYER_FLAG_SPECTATOR)),
			value("GAME_PLAYER_FLAG_SCRIPT_BEGINS_FROM", s32(GAME_PLAYER_FLAG_SCRIPT_BEGINS_FROM))
		],

	class_<enum_exporter<EGamePhases> >("game_phases")
		.enum_("phases")
		[
			value("GAME_PHASE_NONE", s32(GAME_PHASE_NONE)),
			value("GAME_PHASE_INPROGRESS", s32(GAME_PHASE_INPROGRESS)),
			value("GAME_PHASE_PENDING", s32(GAME_PHASE_PENDING)),
			value("GAME_PHASE_TEAM1_SCORES", s32(GAME_PHASE_TEAM1_SCORES)),
			value("GAME_PHASE_TEAM2_SCORES", s32(GAME_PHASE_TEAM2_SCORES)),
			value("GAME_PHASE_TEAMS_IN_A_DRAW", s32(GAME_PHASE_TEAMS_IN_A_DRAW)),
			value("GAME_PHASE_SCRIPT_BEGINS_FROM", s32(GAME_PHASE_SCRIPT_BEGINS_FROM))
		],

	class_<enum_exporter<EGameMessages> >("game_messages")
		.enum_("messages")
		[
			value("GAME_EVENT_PLAYER_READY", s32(GAME_EVENT_PLAYER_READY)),
			value("GAME_EVENT_PLAYER_CHANGE_TEAM", s32(GAME_EVENT_PLAYER_GAME_MENU)),
			value("GAME_EVENT_PLAYER_KILL", s32(GAME_EVENT_PLAYER_KILL)),
			value("GAME_EVENT_PLAYER_BUY_FINISHED", s32(GAME_EVENT_PLAYER_BUY_FINISHED)),
			value("GAME_EVENT_PLAYER_CHANGE_SKIN", s32(GAME_EVENT_PLAYER_GAME_MENU)),
			value("GAME_EVENT_PLAYER_CONNECTED", s32(GAME_EVENT_PLAYER_CONNECTED)),
			value("GAME_EVENT_PLAYER_DISCONNECTED", s32(GAME_EVENT_PLAYER_DISCONNECTED)),
			value("GAME_EVENT_PLAYER_KILLED", s32(GAME_EVENT_PLAYER_KILLED)),
			value("GAME_EVENT_PLAYER_JOIN_TEAM", s32(GAME_EVENT_PLAYER_JOIN_TEAM)),
			value("GAME_EVENT_ROUND_STARTED", s32(GAME_EVENT_ROUND_STARTED)),
			value("GAME_EVENT_ROUND_END", s32(GAME_EVENT_ROUND_END)),
			value("GAME_EVENT_ARTEFACT_SPAWNED", s32(GAME_EVENT_ARTEFACT_SPAWNED)),
			value("GAME_EVENT_ARTEFACT_DESTROYED", s32(GAME_EVENT_ARTEFACT_DESTROYED)),
			value("GAME_EVENT_ARTEFACT_TAKEN", s32(GAME_EVENT_ARTEFACT_TAKEN)),
			value("GAME_EVENT_ARTEFACT_DROPPED", s32(GAME_EVENT_ARTEFACT_DROPPED)),
			value("GAME_EVENT_ARTEFACT_ONBASE", s32(GAME_EVENT_ARTEFACT_ONBASE)),
			value("GAME_EVENT_PLAYER_ENTER_TEAM_BASE", s32(GAME_EVENT_PLAYER_ENTER_TEAM_BASE)),
			value("GAME_EVENT_PLAYER_LEAVE_TEAM_BASE", s32(GAME_EVENT_PLAYER_LEAVE_TEAM_BASE)),
			value("GAME_EVENT_BUY_MENU_CLOSED", s32(GAME_EVENT_BUY_MENU_CLOSED)),
			value("GAME_EVENT_TEAM_MENU_CLOSED", s32(GAME_EVENT_TEAM_MENU_CLOSED)),
			value("GAME_EVENT_SKIN_MENU_CLOSED", s32(GAME_EVENT_SKIN_MENU_CLOSED)),
			value("GAME_EVENT_SCRIPT_BEGINS_FROM", s32(GAME_EVENT_SCRIPT_BEGINS_FROM))
		]
	];
}
