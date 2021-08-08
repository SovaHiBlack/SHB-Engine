#pragma once

#include "Environment.h"
#include "IGameObjectPool.h"

class IMainMenu;
class ENGINE_API CPS_Instance;

class ENGINE_API IGamePersistent :
	public DLL_Pure,
	public pureAppStart,
	public pureAppEnd,
	public pureAppActivate,
	public pureAppDeactivate,
	public pureFrame
{
public:
	union params
	{
		struct
		{
			string256					m_game_or_spawn;
			string256					m_game_type;
			string256					m_alife;
			string256					m_new_or_load;
			u32							m_e_game_type;
		};
		string256						m_params[4];
										params				( )
		{
			reset						( );
		}
		void							reset				( )
		{
			for (int i = 0; i < 4; ++i)
			{
				strcpy_s				(m_params[i], "");
			}
		}
		void							parse_cmd_line		(const char* cmd_line)
		{
										reset				( );
			int n						= _min(4, _GetItemCount(cmd_line, '/'));
			for (int i = 0; i < n; ++i)
			{
				_GetItem				(cmd_line, i, m_params[i], '/');
				strlwr					(m_params[i]);
			}
		}
	};
	params								m_game_params;

	xr_set<CPS_Instance*>				ps_active;
	xr_vector<CPS_Instance*>			ps_destroy;
	xr_vector<CPS_Instance*>			ps_needtoplay;

	void								destroy_particles	(const bool& all_particles);

	virtual void						PreStart			(const char* op);
	virtual void						Start				(const char* op);
	virtual void						Disconnect			( );

	IGameObjectPool					ObjectPool;

	IMainMenu*							m_pMainMenu;

	CEnvironment*						pEnvironment;
	CEnvironment&						Environment			( )
	{
		return *pEnvironment;
	}

	virtual bool						OnRenderPPUI_query	( )
	{	// should return true if we want to have second function called
		return FALSE;
	}
	virtual void						OnRenderPPUI_main	( )
	{ }
	virtual void						OnRenderPPUI_PP		( )
	{ }

	virtual void						OnAppStart			( );
	virtual void						OnAppEnd			( );
	virtual void						OnAppActivate		( );
	virtual void						OnAppDeactivate		( );
	virtual void						OnFrame				( );

	// вызывается только когда изменяется тип игры
	virtual void						OnGameStart			( );
	virtual void						OnGameEnd			( );

	virtual void						UpdateGameType		( )
	{ }

	virtual void						RegisterModel		(IRender_Visual* V) = 0;

	virtual float						MtlTransparent		(u32 mtl_idx) = 0;

	IGamePersistent( );
	virtual								~IGamePersistent( );

	u32									GameType			( )
	{
		return m_game_params.m_e_game_type;
	}
	virtual void						Statistics			(CGameFont* F) = 0;

	virtual void						LoadTitle			(const char* str)
	{ }
};

class IMainMenu
{
public:
	virtual								~IMainMenu			( )
	{ }
	virtual void						Activate			(bool bActive) = 0;
	virtual bool						IsActive			( ) = 0;
	virtual void						DestroyInternal		(bool bForce) = 0;
};

extern ENGINE_API IGamePersistent*		g_pGamePersistent;
