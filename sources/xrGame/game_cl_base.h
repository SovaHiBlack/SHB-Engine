#pragma once

#include "game_base.h"
#include "..\xrNetServer\ClientID.h"
#include "WeaponAmmo.h"

class CNetPacket;
class CGameObject;
class CUIGameCustom;
class CUI;
class CUIDialogWnd;

struct SZoneMapEntityData
{
	fVector3	pos;
	u32		color;
	SZoneMapEntityData( )
	{
		pos.set(.0f, .0f, .0f); color = 0xff00ff00;
	}
	DECLARE_SCRIPT_REGISTER_FUNCTION_STRUCT
};
add_to_type_list(SZoneMapEntityData)
#undef script_type_list
#define script_type_list save_type_list(SZoneMapEntityData)

struct WeaponUsageStatistic;

class	game_cl_GameState : public game_GameState, public ISheduled
{
	typedef game_GameState	inherited;
	shared_str							m_game_type_name;

protected:
	CUIGameCustom* m_game_ui_custom;
	u16									m_u16VotingEnabled;
	bool								m_bServerControlHits;

public:
	typedef xr_map<CClientID, game_PlayerState*>	PLAYERS_MAP;
	typedef PLAYERS_MAP::iterator				PLAYERS_MAP_IT;
	typedef PLAYERS_MAP::const_iterator			PLAYERS_MAP_CIT;

	PLAYERS_MAP							players;
	CClientID							local_svdpnid;
	game_PlayerState* local_player;

	WeaponUsageStatistic* m_WeaponUsageStatistic;
	virtual		void				reset_ui( );
	virtual		void				CommonMessageOut(pcstr msg);

private:
	void				switch_Phase(u32 new_phase)
	{
		inherited::switch_Phase(new_phase);
	}

protected:
	virtual		void				OnSwitchPhase(u32 old_phase, u32 new_phase);

	//for scripting enhancement
	virtual		void				TranslateGameMessage(u32 msg, CNetPacket& P);

	virtual		shared_str			shedule_Name( ) const
	{
		return shared_str("game_cl_GameState");
	}
	virtual		f32					shedule_Scale( );
	virtual		bool				shedule_Needed( )
	{
		return true;
	}

	void				sv_GameEventGen(CNetPacket& P);
	void				sv_EventSend(CNetPacket& P);

public:
	game_cl_GameState( );
	virtual							~game_cl_GameState( );
	pcstr				type_name( ) const
	{
		return *m_game_type_name;
	}
	void				set_type_name(pcstr s);
	virtual		void				Init( )
	{ }
	virtual		void				net_import_state(CNetPacket& P);
	virtual		void				net_import_update(CNetPacket& P);
	virtual		void				net_import_GameTime(CNetPacket& P);						// update GameTime only for remote clients
	virtual		void				net_signal(CNetPacket& P);

	bool				IR_OnKeyboardPress(s32 dik);
	bool				IR_OnKeyboardRelease(s32 dik);
	bool				IR_OnMouseMove(s32 dx, s32 dy);
	bool				IR_OnMouseWheel(s32 direction);

	virtual		bool				OnKeyboardPress(s32 key)
	{
		return false;
	}
	virtual		bool				OnKeyboardRelease(s32 key)
	{
		return false;
	}
	void				OnGameMessage(CNetPacket& P);

	virtual		pstr				getTeamSection(s32 Team)
	{
		return NULL;
	}

	game_PlayerState* GetPlayerByGameID(u32 GameID);
	game_PlayerState* GetPlayerByOrderID(u32 id);
	CClientID			GetClientIDByOrderID(u32 id);
	u32					GetPlayersCount( ) const
	{
		return players.size( );
	}
	virtual		CUIGameCustom* createGameUI( )
	{
		return NULL;
	}
	virtual		void				GetMapEntities(xr_vector<SZoneMapEntityData>& dst)
	{ }

	void				StartStopMenu(CUIDialogWnd* pDialog, bool bDoHideIndicators);
	virtual		void				shedule_Update(u32 dt);

	void				u_EventGen(CNetPacket& P, u16 type, u16 dest);
	void				u_EventSend(CNetPacket& P);

	virtual		void				ChatSayTeam(const shared_str& phrase)
	{ }
	virtual		void				ChatSayAll(const shared_str& phrase)
	{ }
	virtual		void				OnChatMessage(CNetPacket* P)
	{ }
	virtual		void				OnWarnMessage(CNetPacket* P)
	{ }
	virtual		void				OnRadminMessage(u16 type, CNetPacket* P)
	{ }

	virtual		bool				IsVotingEnabled( )
	{
		return m_u16VotingEnabled != 0;
	}
	virtual		bool				IsVotingEnabled(u16 flag)
	{
		return (m_u16VotingEnabled & flag) != 0;
	}
	virtual		bool				IsVotingActive( )
	{
		return false;
	}
	virtual		void				SetVotingActive(bool Active)
	{ }
	virtual		void				SendStartVoteMessage(pcstr args)
	{ }
	virtual		void				SendVoteYesMessage( )
	{ }
	virtual		void				SendVoteNoMessage( )
	{ }
	virtual		void				OnVoteStart(CNetPacket& P)
	{ }
	virtual		void				OnVoteStop(CNetPacket& P)
	{ }

	virtual		void				OnRender( )
	{ }
	virtual		bool				IsServerControlHits( )
	{
		return m_bServerControlHits;
	}
	virtual		bool				IsEnemy(game_PlayerState* ps)
	{
		return false;
	}
	virtual		bool				IsEnemy(CEntityAlive* ea1, CEntityAlive* ea2)
	{
		return false;
	}
	virtual		bool				PlayerCanSprint(CActor* pActor)
	{
		return true;
	}

	virtual		void				OnSpawn(CObject* pObj)
	{ }
	virtual		void				OnDestroy(CObject* pObj)
	{ }

	virtual		void				OnPlayerFlagsChanged(game_PlayerState* ps)
	{ }
	virtual		void				OnPlayerVoted(game_PlayerState* ps)
	{ }
	virtual		void				SendPickUpEvent(u16 ID_who, u16 ID_what);
};
