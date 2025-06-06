#pragma once

#include "..\XR_3DA\IGame_Persistent.h"
class CMainMenu;
class CUICursor;
class CParticlesObject;
class CUISequencer;
class ui_core;

class CGamePersistent : public IGame_Persistent, public IEventReceiver
{
	// ambient particles
	CParticlesObject* ambient_particles;
	u32					ambient_sound_next_time;
	u32					ambient_effect_next_time;
	u32					ambient_effect_stop_time;

	CUISequencer* m_intro;
	EVENT				eQuickLoad;

	fastdelegate::FastDelegate0<> m_intro_event;

	void xr_stdcall		start_logo_intro( );
	void xr_stdcall		update_logo_intro( );
	void xr_stdcall		start_game_intro( );
	void xr_stdcall		update_game_intro( );

#ifdef DEBUG
	u32					m_frame_counter;
	u32					m_last_stats_frame;
#endif // def DEBUG

	void				WeathersUpdate( );

public:
	ui_core* m_pUI_core;
	IReader* pDemoFile;
	u32					uTime2Change;
	EVENT				eDemoStart;

	CGamePersistent( );
	virtual				~CGamePersistent( );

	virtual void		Start(pcstr op);
	virtual void		Disconnect( );

	virtual void		OnAppActivate( );
	virtual void		OnAppDeactivate( );

	virtual void		OnAppStart( );
	virtual void		OnAppEnd( );
	virtual void		OnGameStart( );
	virtual void		OnGameEnd( );
	virtual void		OnFrame( );
	virtual void		OnEvent(EVENT E, u64 P1, u64 P2);

	virtual void		UpdateGameType( );

	virtual void		RegisterModel(IRenderVisual* V);
	virtual f32			MtlTransparent(u32 mtl_idx);
	virtual void		Statistics(CGameFont* F);

	virtual bool		OnRenderPPUI_query( );
	virtual void		OnRenderPPUI_main( );
	virtual void		OnRenderPPUI_PP( );
	virtual void		LoadTitle(pcstr str);
};

IC CGamePersistent& GamePersistent( )
{
	return *((CGamePersistent*) g_pGamePersistent);
}
