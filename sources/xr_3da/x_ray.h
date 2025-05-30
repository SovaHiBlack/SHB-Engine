#pragma once

// refs
class ENGINE_API CGameFont;

// definition
class ENGINE_API CApplication : public pureFrame, public IEventReceiver
{
	// levels
	struct					sLevelInfo
	{
		pstr				folder;
		pstr				name;
	};
	string256				app_title;

private:
	ref_shader				hLevelLogo;
	ref_geom				ll_hGeom;
	ref_geom				ll_hGeom2;

	ref_shader				sh_progress;
	s32						load_stage;

	u32						ll_dwReference;

private:
	EVENT					eQuit;
	EVENT					eStart;
	EVENT					eStartLoad;
	EVENT					eDisconnect;

	void					Level_Append(pcstr lname);

public:
	CGameFont* pFontSystem;

	// Levels
	xr_vector<sLevelInfo>	Levels;
	u32						Level_Current;
	void					Level_Scan( );
	s32						Level_ID(pcstr name);
	void					Level_Set(u32 ID);

	// Loading
	void					LoadBegin( );
	void					LoadEnd( );
	void					LoadTitleInt(pcstr str);
	void					LoadDraw( );

	virtual void			OnEvent(EVENT E, u64 P1, u64 P2);

	// Other
	CApplication( );
	~CApplication( );

	virtual void			OnFrame( );
	void			load_draw_internal( );
	void			destroy_loading_shaders( );
};

extern ENGINE_API	CApplication* pApp;
