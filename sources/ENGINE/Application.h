#pragma once

// refs
class ENGINE_API CGameFont;

// definition
class ENGINE_API CApplication : public pureFrame, public IEventReceiver
{
	// levels
	struct							SLevelInfo
	{
		char*						folder;
		char*						name;
	};
	string256						app_title;

private:
	ref_shader						hLevelLogo;
	ref_geom						ll_hGeom;
	ref_geom						ll_hGeom2;

	ref_shader						sh_progress;
	int								load_stage;

	unsigned int								ll_dwReference;

	EVENT							eQuit;
	EVENT							eStart;
	EVENT							eStartLoad;
	EVENT							eDisconnect;

	void							Level_Append				(const char* folder);

public:
	CGameFont*						pFontSystem;

	// Levels
	xr_vector<SLevelInfo>			Levels;
	unsigned int								Level_Current;
	void							Level_Scan					( );
	int								Level_ID					(const char* name);
	void							Level_Set					(unsigned int ID);

	// Loading
	void							LoadBegin					( );
	void							LoadEnd						( );
	void							LoadTitleInt				(const char* str);
	void							LoadDraw					( );

	virtual void					OnEvent						(EVENT E, unsigned __int64 P1, unsigned __int64 P2);

	// Other
									CApplication				( );
									~CApplication				( );

	virtual void					OnFrame						( );
	void							load_draw_internal			( );
	void							destroy_loading_shaders		( );
};

extern ENGINE_API CApplication*		pApp;
