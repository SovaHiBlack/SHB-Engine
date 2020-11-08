#include "stdafx.h"

#include "IGameLevel.h"
#include "IGamePersistent.h"//==>
#include "Application.h"
#include "std_classes.h"
#include "CustomHUD.h"
#include "render.h"
#include "gamefont.h"
#include "xrLevel.h"
#include "CameraManager.h"
#include "ResourceManager.h"

ENGINE_API	IGameLevel*	g_pGameLevel	= nullptr;

IGameLevel::IGameLevel()
{
	m_pCameras					= xr_new<CCameraManager>(true);
	g_pGameLevel				= this;
	pLevel						= NULL;
	bReady						= false;
	pCurrentEntity				= NULL;
	pCurrentViewEntity			= NULL;
	pHUD						= NULL;
}

IGameLevel::~IGameLevel()
{
	if(strstr(Core.Params,"-nes_texture_storing") )
		Device.Resources->StoreNecessaryTextures();

	xr_delete					( pLevel		);

	// Render-level unload
	Render->level_Unload		();
	xr_delete					(m_pCameras);
	// Unregister
	Device.seqRender.Remove		(this);
	Device.seqFrame.Remove		(this);
	CCameraManager::ResetPP		();
}

void IGameLevel::net_Stop			()
{
	for (int i=0; i<6; i++)
		Objects.Update(true);
	// Destroy all objects
	Objects.Unload				( );
	IR_Release					( );

	bReady						= false;
}

//-------------------------------------------------------------------------------------------
extern CStatTimer				tscreate;

BOOL IGameLevel::Load			(u32 dwNum)
{
	// Initialize level data
	pApp->Level_Set				( dwNum );
	string_path					temp;
	if (!FS.exist(temp, "$level$", "level.ltx"))
		Debug.fatal	(DEBUG_INFO,"Can't find level configuration file '%s'.",temp);
	pLevel						= xr_new<CIniFile>	( temp );
	
	// Open
	g_pGamePersistent->LoadTitle	("st_opening_stream");
	IReader* LL_Stream			= FS.r_open	("$level$","level");
	IReader	&fs					= *LL_Stream;

	// Header
	hdrLEVEL					H;
	fs.r_chunk_safe				(fsL_HEADER,&H,sizeof(H));
	R_ASSERT2					(XRCL_PRODUCTION_VERSION==H.XRLC_version,"Incompatible level version.");

	// CForms
	g_pGamePersistent->LoadTitle	("st_loading_cform");
	ObjectSpace.Load			();

	// HUD + Environment
	if(g_hud)
		pHUD					= g_hud;
	else
		pHUD					= (CCustomHUD*)NEW_INSTANCE	(CLSID_HUDMANAGER);

	// Render-level Load
	Render->level_Load			(LL_Stream);
	tscreate.FrameEnd			();
	// Msg						("* S-CREATE: %f ms, %d times",tscreate.result,tscreate.count);

	// Objects
	g_pGamePersistent->Environment().mods_load	();
	R_ASSERT					(Load_GameSpecific_Before());
	Objects.Load				();
	R_ASSERT					(Load_GameSpecific_After ());

	// Done
	FS.r_close					( LL_Stream );
	bReady						= true;
	IR_Capture					();

	Device.seqRender.Add		(this);
	Device.seqFrame.Add			(this);

	return TRUE;
}

int		psNET_DedicatedSleep	= 5;
void	IGameLevel::OnRender		( )
{
//	if (_abs(Device.fTimeDelta)<EPS_S) return;

	// Level render, only when no client output required
	Render->Calculate			();
	Render->Render				();

	// Font
//	pApp->pFontSystem->SetSizeI(0.023f);
//	pApp->pFontSystem->OnRender	();
}

void	IGameLevel::OnFrame		( )
{
	// Log				("- level:on-frame: ",u32(Device.dwFrame));
//	if (_abs(Device.fTimeDelta)<EPS_S) return;

	// Update all objects
	VERIFY						(bReady);
	Objects.Update				( false );
	pHUD->OnFrame				( );

	// Ambience
	if (Sounds_Random.size() && (Device.dwTimeGlobal > Sounds_Random_dwNextTime))
	{
		Sounds_Random_dwNextTime		= Device.dwTimeGlobal + ::Random.randI	(10000,20000);
		Fvector	pos;
		pos.random_dir().normalize().mul(::Random.randF(30,100)).add	(Device.vCameraPosition);
		int		id						= ::Random.randI(Sounds_Random.size());
		if (Sounds_Random_Enabled)		{
			Sounds_Random[id].play_at_pos	(0,pos,0);
			Sounds_Random[id].set_volume	(1.f);
			Sounds_Random[id].set_range		(10,200);
		}
	}
}

// ==================================================================================================

void CServerInfo::AddItem(const char* name_, const char* value_, u32 color_ )
{
	shared_str s_name( name_ );
	AddItem( s_name, value_, color_ );
}

void CServerInfo::AddItem( shared_str& name_, const char* value_, u32 color_ )
{
	SItem_ServerInfo it;
//	shared_str s_name = CStringTable().translate( name_ );
	
//	strcpy_s( it.name, s_name.c_str() );
	strcpy_s( it.name, name_.c_str() );
	strcat_s( it.name, " = " );
	strcat_s( it.name, value_ );
	it.color = color_;

	if ( data.size() < max_item )
	{
		data.push_back( it );
	}
}

void IGameLevel::LL_CheckTextures( )
{
	u32	m_base, c_base, m_lmaps, c_lmaps;
	Device.Resources->_GetMemoryUsage(m_base, c_base, m_lmaps, c_lmaps);

	Msg("* t-report - base: %d, %d K", c_base, m_base / 1024);
	Msg("* t-report - lmap: %d, %d K", c_lmaps, m_lmaps / 1024);
	BOOL	bError = FALSE;
	if (m_base > 64 * 1024 * 1024 || c_base > 400)
	{
		// const char* msg	= "Too many base-textures (limit: 400 textures or 64M).\n        Reduce number of textures (better) or their resolution (worse).";
		// Msg		("***FATAL***: %s",msg);
		bError = TRUE;
	}
	if (m_lmaps > 32 * 1024 * 1024 || c_lmaps > 8)
	{
		const char* msg = "Too many lmap-textures (limit: 8 textures or 32M).\n        Reduce pixel density (worse) or use more vertex lighting (better).";
		Msg("***FATAL***: %s", msg);
		bError = TRUE;
	}
}
