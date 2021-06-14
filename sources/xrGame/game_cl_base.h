#pragma once

#include "game_base.h"
#include "..\ENGINE\ClientID.h"
#include "WeaponAmmo.h"
//#include "Level_Bullet_Manager.h"

class CNetPacket;
class CGameObject;
class CUIGameCustom;
class CUI;
class CUIDialogWnd;

struct SZoneMapEntityData
{
	Fvector3	pos;
	u32		color;
	SZoneMapEntityData( )
	{
		pos.set(0.0f, 0.0f, 0.0f);
		color = 0xff00ff00;
	}

	DECLARE_SCRIPT_REGISTER_FUNCTION_STRUCT
};

add_to_type_list(SZoneMapEntityData)
#undef script_type_list
#define script_type_list save_type_list(SZoneMapEntityData)

//#include "game_cl_base_weapon_usage_statistic.h"

struct WeaponUsageStatistic;

class game_cl_GameState : public game_GameState, public ISheduled
{
	using inherited											= game_GameState;
	shared_str												m_game_type_name;

protected:
	CUIGameCustom*											m_game_ui_custom;
	U16														m_u16VotingEnabled;
	bool													m_bServerControlHits;

public:
	using PLAYERS_MAP										= xr_map<ClientID, game_PlayerState*>;
	using PLAYERS_MAP_IT									= PLAYERS_MAP::iterator;
	using PLAYERS_MAP_CIT									= PLAYERS_MAP::const_iterator;

	PLAYERS_MAP												players;
	ClientID												local_svdpnid;
	game_PlayerState*										local_player;

	WeaponUsageStatistic*									m_WeaponUsageStatistic;
	virtual void				reset_ui					( );
	virtual void				CommonMessageOut			(const char* msg);

private:
	void						switch_Phase				(u32 new_phase)
	{
		inherited::switch_Phase(new_phase);
	}

protected:
	virtual void				OnSwitchPhase				(u32 old_phase, u32 new_phase);

	//for scripting enhancement
	virtual void				TranslateGameMessage		(u32 msg, CNetPacket& P);

	virtual shared_str			shedule_Name				( ) const
	{
		return shared_str("game_cl_GameState");
	}
	virtual float				shedule_Scale				( );
	virtual bool				shedule_Needed				( )
	{
		return true;
	}

	void						sv_GameEventGen				(CNetPacket& P);
	void						sv_EventSend				(CNetPacket& P);

public:
								game_cl_GameState			( );
	virtual						~game_cl_GameState			( );
	const char*					type_name					( ) const
	{
		return *m_game_type_name;
	}
	void						set_type_name				(const char* s);
	virtual void				Init						( )
	{ }
	virtual void				net_import_state			(CNetPacket& P);
	virtual void				net_import_update			(CNetPacket& P);
	virtual void				net_import_GameTime			(CNetPacket& P);						// update GameTime only for remote clients
	virtual void				net_signal					(CNetPacket& P);

	bool						IR_OnKeyboardPress			(int dik);
	bool						IR_OnKeyboardRelease		(int dik);
	bool						IR_OnMouseMove				(int dx, int dy);
	bool						IR_OnMouseWheel				(int direction);

	virtual bool				OnKeyboardPress				(int key)
	{
		return false;
	}
	virtual bool				OnKeyboardRelease			(int key)
	{
		return false;
	}
	void						OnGameMessage				(CNetPacket& P);

	virtual char*				getTeamSection				(int Team)
	{
		return nullptr;
	}

	game_PlayerState*			GetPlayerByGameID			(u32 GameID);
	game_PlayerState*			GetPlayerByOrderID			(u32 id);
	ClientID					GetClientIDByOrderID		(u32 id);
	u32							GetPlayersCount				( ) const
	{
		return players.size( );
	}
	virtual CUIGameCustom*		createGameUI				( )
	{
		return nullptr;
	}
	virtual void				GetMapEntities				(xr_vector<SZoneMapEntityData>& dst)
	{ }

	void						StartStopMenu				(CUIDialogWnd* pDialog, bool bDoHideIndicators);
	virtual void				shedule_Update				(u32 dt);

	void						u_EventGen					(CNetPacket& P, U16 type, U16 dest);
	void						u_EventSend					(CNetPacket& P);

	virtual void				OnChatMessage				(CNetPacket* P)
	{ }
	virtual void				OnWarnMessage				(CNetPacket* P)
	{ }
	virtual void				OnRadminMessage				(U16 type, CNetPacket* P)
	{ }

	virtual void				OnRender					( )
	{ }
	virtual bool				IsServerControlHits			( )
	{
		return m_bServerControlHits;
	}
	virtual bool				IsEnemy						(game_PlayerState* ps)
	{
		return false;
	}
	virtual bool				IsEnemy						(CEntityAlive* ea1, CEntityAlive* ea2)
	{
		return false;
	}
	virtual bool				PlayerCanSprint				(CActor* pActor)
	{
		return true;
	}

	virtual void				OnSpawn						(CObject* pObj)
	{ }
	virtual void				OnDestroy					(CObject* pObj)
	{ }

	virtual void				SendPickUpEvent				(U16 ID_who, U16 ID_what);
};
