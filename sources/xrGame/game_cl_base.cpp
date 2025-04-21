#include "stdafx.h"

#include "hudmanager.h"
#include "game_cl_base.h"
#include "level.h"
#include "GamePersistent.h"
#include "UIGameCustom.h"
#include "script_engine.h"
#include "xr_Level_controller.h"
#include "ui/UIMainIngameWnd.h"//
#include "UI/UIGameTutorial.h"
#include "UI/UIMessagesWindow.h"//
#include "string_table.h"
#include "game_cl_base_weapon_usage_statistic.h"

game_cl_GameState::game_cl_GameState( )
{
	m_WeaponUsageStatistic = xr_new<WeaponUsageStatistic>( );

	local_player = 0;
	m_game_type_name = 0;

	shedule.t_min = 5;
	shedule.t_max = 20;
	m_game_ui_custom = NULL;
	shedule_register( );

	m_u16VotingEnabled = 0;
	m_bServerControlHits = true;
}

game_cl_GameState::~game_cl_GameState( )
{
	PLAYERS_MAP_IT I = players.begin( );
	for (; I != players.end( ); ++I)
	{
		xr_delete(I->second);
	}

	players.clear( );

	shedule_unregister( );

	xr_delete(m_WeaponUsageStatistic);
}

void	game_cl_GameState::net_import_GameTime(CNetPacket& P)
{
	//time
	u64				GameTime;
	P.r_u64(GameTime);
	f32			TimeFactor;
	P.r_float(TimeFactor);

	Level( ).SetGameTimeFactor(GameTime, TimeFactor);

	u64				GameEnvironmentTime;
	P.r_u64(GameEnvironmentTime);
	f32			EnvironmentTimeFactor;
	P.r_float(EnvironmentTimeFactor);

// KRodin: закомментировано из-за бага с резкой сменой погоды при кручении таймфактора.
//	u64 OldTime = Level().GetEnvironmentGameTime();
	Level( ).SetEnvironmentGameTimeFactor(GameEnvironmentTime, EnvironmentTimeFactor);
//	if (OldTime > GameEnvironmentTime)
//		GamePersistent().Environment().Invalidate();
}

void game_cl_GameState::net_import_state(CNetPacket& P)
{
	// Generic
	P.r_clientID(local_svdpnid);
	P.r_s32(m_type);

	u16 ph;
	P.r_u16(ph);

	if (Phase( ) != ph)
	{
		switch_Phase(ph);
	}

	P.r_s32(m_round);
	P.r_u32(m_start_time);
	m_u16VotingEnabled = u16(P.r_u8( ));
	m_bServerControlHits = !!P.r_u8( );
	m_WeaponUsageStatistic->SetCollectData(!!P.r_u8( ));

	// Players
	u16	p_count;
	P.r_u16(p_count);

	PLAYERS_MAP players_new;

/*
	players.clear	();
*/
	PLAYERS_MAP_IT I;
	for (u16 p_it = 0; p_it < p_count; ++p_it)
	{
		CClientID ID;
		P.r_clientID(ID);

		game_PlayerState* IP;
		I = players.find(ID);
		if (I != players.end( ))
		{
			IP = I->second;
			IP->net_Import(P);

			players_new.insert(mk_pair(ID, IP));
			players.erase(I);
		}
		else
		{
			IP = createPlayerState( );
			IP->net_Import(P);

			players_new.insert(mk_pair(ID, IP));
		}

		if (IP->testFlag(GAME_PLAYER_FLAG_LOCAL))
		{
			local_player = IP;
		}
	}

	I = players.begin( );
	for (; I != players.end( ); ++I)
	{
		xr_delete(I->second);
	}

	players.clear( );

	players = players_new;

	net_import_GameTime(P);
}

void game_cl_GameState::net_import_update(CNetPacket& P)
{
	// Read
	CClientID ID;
	P.r_clientID(ID);

	// Update
	PLAYERS_MAP_IT I = players.find(ID);
	if (players.end( ) != I)
	{
		game_PlayerState* IP = I->second;
		IP->net_Import(P);
	}
	else
	{
		game_PlayerState* PS = createPlayerState( );
		PS->net_Import(P);
		xr_delete(PS);
	}

	//Syncronize GameTime
	net_import_GameTime(P);
}

void game_cl_GameState::net_signal(CNetPacket& P)
{ }

void game_cl_GameState::TranslateGameMessage(u32 msg, CNetPacket& P)
{
	CStringTable st;

	string512 Text;
	char Color_Main[ ] = "%c[255,192,192,192]";
	pstr Color_Teams[3] = {"%c[255,255,240,190]", "%c[255,64,255,64]", "%c[255,64,64,255]"};

	switch (msg)
	{
		case GAME_EVENT_PLAYER_CONNECTED:
		{
			string64 PlayerName;
			P.r_stringZ(PlayerName);

			sprintf_s(Text, "%s%s %s%s", Color_Teams[0], PlayerName, Color_Main, *st.translate("mp_connected"));
			CommonMessageOut(Text);
			//---------------------------------------
			Msg("%s connected", PlayerName);
		}
		break;
		case GAME_EVENT_PLAYER_DISCONNECTED:
		{
			string64 PlayerName;
			P.r_stringZ(PlayerName);

			sprintf_s(Text, "%s%s %s%s", Color_Teams[0], PlayerName, Color_Main, *st.translate("mp_disconnected"));
			CommonMessageOut(Text);
			//---------------------------------------
			Msg("%s disconnected", PlayerName);
		}
		break;
		case GAME_EVENT_PLAYER_ENTERED_GAME:
		{
			string64 PlayerName;
			P.r_stringZ(PlayerName);

			sprintf_s(Text, "%s%s %s%s", Color_Teams[0], PlayerName, Color_Main, *st.translate("mp_entered_game"));
			CommonMessageOut(Text);
		}
		break;
		default:
		{
			R_ASSERT2(0, "Unknown Game Message");
		}
		break;
	}
}

void game_cl_GameState::OnGameMessage(CNetPacket& P)
{
	VERIFY(this && &P);
	u32 msg;
	P.r_u32(msg);

	TranslateGameMessage(msg, P);
}

game_PlayerState* game_cl_GameState::GetPlayerByGameID(u32 GameID)
{
	PLAYERS_MAP_IT I = players.begin( );
	PLAYERS_MAP_IT E = players.end( );

	for (; I != E; ++I)
	{
		game_PlayerState* P = I->second;
		if (P->GameID == GameID)
		{
			return P;
		}
	}

	return NULL;
}

game_PlayerState* game_cl_GameState::GetPlayerByOrderID(u32 idx)
{
	PLAYERS_MAP_IT I = players.begin( );
	std::advance(I, idx);
	game_PlayerState* ps = I->second;
	return ps;
}

CClientID game_cl_GameState::GetClientIDByOrderID(u32 idx)
{
	PLAYERS_MAP_IT I = players.begin( );
	std::advance(I, idx);
	return I->first;
}

void game_cl_GameState::CommonMessageOut(pcstr msg)
{
	if (!HUD( ).GetUI( )) return;
	HUD( ).GetUI( )->m_pMessagesWnd->AddLogMessage(msg);
}

f32 game_cl_GameState::shedule_Scale( )
{
	return 1.0f;
}

void game_cl_GameState::shedule_Update(u32 dt)
{
	ISheduled::shedule_Update(dt);

	if (!m_game_ui_custom)
	{
		if (HUD( ).GetUI( ))
		{
			m_game_ui_custom = HUD( ).GetUI( )->UIGame( );
		}
	}
}

void game_cl_GameState::StartStopMenu(CUIDialogWnd* pDialog, bool bDoHideIndicators)
{
	HUD( ).GetUI( )->StartStopMenu(pDialog, bDoHideIndicators);
}

void game_cl_GameState::sv_GameEventGen(CNetPacket& P)
{
	P.w_begin(M_EVENT);
	P.w_u32(Level( ).timeServer( ));
	P.w_u16(u16(GE_GAME_EVENT & 0xffff));
	P.w_u16(0);//dest==0
}

void game_cl_GameState::sv_EventSend(CNetPacket& P)
{
	Level( ).Send(P, net_flags(TRUE, TRUE));
}

bool game_cl_GameState::IR_OnKeyboardPress(s32 dik)
{
	if (local_player && !local_player->IsSkip( ))
		return OnKeyboardPress(get_binded_action(dik));
	else
		return false;
}

bool game_cl_GameState::IR_OnKeyboardRelease(s32 dik)
{
	if (local_player && !local_player->IsSkip( ))
	{
		return OnKeyboardRelease(get_binded_action(dik));
	}
	else
	{
		return false;
	}
}

bool game_cl_GameState::IR_OnMouseMove(s32 dx, s32 dy)
{
	return false;
}

bool game_cl_GameState::IR_OnMouseWheel(s32 direction)
{
	return false;
}

void game_cl_GameState::u_EventGen(CNetPacket& P, u16 type, u16 dest)
{
	P.w_begin(M_EVENT);
	P.w_u32(Level( ).timeServer( ));
	P.w_u16(type);
	P.w_u16(dest);
}

void game_cl_GameState::u_EventSend(CNetPacket& P)
{
	Level( ).Send(P, net_flags(TRUE, TRUE));
}

void game_cl_GameState::OnSwitchPhase(u32 old_phase, u32 new_phase)
{
	switch (old_phase)
	{
		case GAME_PHASE_INPROGRESS:
		{
		}
		break;
		default:
		{
		}
		break;
	}

	switch (new_phase)
	{
		case GAME_PHASE_INPROGRESS:
		{
			m_WeaponUsageStatistic->Clear( );
		}
		break;
		default:
		{
		}
		break;
	}
}

void game_cl_GameState::SendPickUpEvent(u16 ID_who, u16 ID_what)
{
	CNetPacket P;
	u_EventGen(P, GE_OWNERSHIP_TAKE, ID_who);
	P.w_u16(ID_what);
	u_EventSend(P);
}

void game_cl_GameState::set_type_name(pcstr s)
{
	m_game_type_name = s;
	if (OnClient( ))
	{
		strcpy(g_pGamePersistent->m_game_params.m_game_type, *m_game_type_name);
		g_pGamePersistent->OnGameStart( );
	}
}

void game_cl_GameState::reset_ui( )
{
	if (!m_game_ui_custom)
	{
		m_game_ui_custom = HUD( ).GetUI( )->UIGame( );
	}

	m_game_ui_custom->reset_ui( );

	HUD( ).GetUI( )->UIMainIngameWnd->reset_ui( );

	if (HUD( ).GetUI( )->MainInputReceiver( ))
	{
		HUD( ).GetUI( )->StartStopMenu(HUD( ).GetUI( )->MainInputReceiver( ), true);
	}
}
