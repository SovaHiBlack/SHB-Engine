#include "stdafx.h"
#include "HUDManager.h"
#include "hudtarget.h"

#include "Actor.h"
#include "..\XR_3DA\igame_level.h"
#include "clsid_game.h"
#include "GamePersistent.h"
#include "ui\UIMainInGameWnd.h"//
#include "StringTable.h"

CFontManager::CFontManager( )
{
	Device.seqDeviceReset.Add(this, REG_PRIORITY_HIGH);

	m_all_fonts.push_back(&pFontSmall);
	m_all_fonts.push_back(&pFontMedium);// used cpp
	m_all_fonts.push_back(&pFontDI);// used cpp
	m_all_fonts.push_back(&pFontArial14);// used xml
	m_all_fonts.push_back(&pFontArial21);
	m_all_fonts.push_back(&pFontGraffiti19Russian);
	m_all_fonts.push_back(&pFontGraffiti22Russian);
	m_all_fonts.push_back(&pFontGraffiti32Russian);
	m_all_fonts.push_back(&pFontGraffiti40Russian);
	m_all_fonts.push_back(&pFontGraffiti50Russian);
	m_all_fonts.push_back(&pFontLetterica16Russian);
	m_all_fonts.push_back(&pFontLetterica18Russian);
	m_all_fonts.push_back(&pFontLetterica25Russian);

	FONTS_VEC_IT it = m_all_fonts.begin( );
	FONTS_VEC_IT it_e = m_all_fonts.end( );
	for (; it != it_e; ++it)
	{
		(**it) = NULL;
	}

	InitializeFonts( );
}

void CFontManager::InitializeFonts( )
{
	InitializeFont(pFontSmall, "hud_font_small", CGameFont::fsDeviceIndependent);
	InitializeFont(pFontMedium, "hud_font_medium");
	InitializeFont(pFontDI, "hud_font_di", CGameFont::fsGradient | CGameFont::fsDeviceIndependent);
	InitializeFont(pFontArial14, "ui_font_arial_14");
	InitializeFont(pFontArial21, "ui_font_arial_21");
	InitializeFont(pFontGraffiti19Russian, "ui_font_graffiti19_russian");
	InitializeFont(pFontGraffiti22Russian, "ui_font_graffiti22_russian");
	InitializeFont(pFontGraffiti32Russian, "ui_font_graffiti32_russian");
	InitializeFont(pFontGraffiti40Russian, "ui_font_graffiti40_russian");
	InitializeFont(pFontGraffiti50Russian, "ui_font_graffiti50_russian");
	InitializeFont(pFontLetterica16Russian, "ui_font_letterica16_russian");
	InitializeFont(pFontLetterica18Russian, "ui_font_letterica18_russian");
	InitializeFont(pFontLetterica25Russian, "ui_font_letterica25_russian");
}

pcstr CFontManager::GetFontTexName(pcstr section)
{
	static pstr tex_names[ ] = { "texture800", "texture", "texture1600" };
	s32 def_idx = 1;//default 1024x768
	s32 idx = def_idx;

#if 0
	u32 w = Device.dwWidth;
	if (w <= 800)
	{
		idx = 0;
	}
	else if (w <= 1280)
	{
		idx = 1;
	}
	else
	{
		idx = 2;
	}
#else
	u32 h = Device.dwHeight;
	if (h <= 600)
	{
		idx = 0;
	}
	else if (h <= 900)
	{
		idx = 1;
	}
	else
	{
		idx = 2;
	}
#endif

	while (idx >= 0)
	{
		if (pSettings->line_exist(section, tex_names[idx]))
		{
			return pSettings->r_string(section, tex_names[idx]);
		}

		--idx;
	}

	return pSettings->r_string(section, tex_names[def_idx]);
}

void CFontManager::InitializeFont(CGameFont*& F, pcstr section, u32 flags)
{
	pcstr font_tex_name = GetFontTexName(section);
	R_ASSERT(font_tex_name);

	if (!F)
	{
		F = xr_new<CGameFont>("font", font_tex_name, flags);
	}
	else
	{
		F->Initialize("font", font_tex_name);
	}

#ifdef DEBUG
	F->m_font_name = section;
#endif

	if (pSettings->line_exist(section, "size"))
	{
		f32 sz = pSettings->r_float(section, "size");
		if (flags & CGameFont::fsDeviceIndependent)
		{
			F->SetHeightI(sz);
		}
		else
		{
			F->SetHeight(sz);
		}
	}

	if (pSettings->line_exist(section, "interval"))
	{
		F->SetInterval(pSettings->r_fvector2(section, "interval"));
	}
}

CFontManager::~CFontManager( )
{
	Device.seqDeviceReset.Remove(this);
	FONTS_VEC_IT it = m_all_fonts.begin( );
	FONTS_VEC_IT it_e = m_all_fonts.end( );
	for (; it != it_e; ++it)
	{
		xr_delete(**it);
	}
}

void CFontManager::Render( )
{
	FONTS_VEC_IT it = m_all_fonts.begin( );
	FONTS_VEC_IT it_e = m_all_fonts.end( );
	for (; it != it_e; ++it)
	{
		(**it)->OnRender( );
	}
}

void CFontManager::OnDeviceReset( )
{
	InitializeFonts( );
}

CHUDManager::CHUDManager( )
{
	pUI = 0;
	m_pHUDTarget = xr_new<CHUDTarget>( );
	OnDisconnected( );
}

CHUDManager::~CHUDManager( )
{
	xr_delete(pUI);
	xr_delete(m_pHUDTarget);
	b_online = false;
}

void CHUDManager::Load( )
{
	if (pUI)
	{
		pUI->Load(pUI->UIGame( ));
		return;
	}

	pUI = xr_new<CUI>(this);
	pUI->Load(NULL);
	OnDisconnected( );
}

void CHUDManager::OnFrame( )
{
	if (!b_online)
	{
		return;
	}

	if (pUI)
	{
		pUI->UIOnFrame( );
	}

	m_pHUDTarget->CursorOnFrame( );
}

ENGINE_API extern f32 psHUD_FOV;

void CHUDManager::Render_First( )
{
	if (!psHUD_Flags.is(HUD_WEAPON | HUD_WEAPON_RT))
	{
		return;
	}

	if (0 == pUI)
	{
		return;
	}

	CObject* O = g_pGameLevel->CurrentViewEntity( );
	if (0 == O)
	{
		return;
	}

	CActor* A = smart_cast<CActor*> (O);
	if (!A)
	{
		return;
	}

	if (A && !A->HUDview( ))
	{
		return;
	}

	// only shadow
	::Render->set_Invisible(TRUE);
	::Render->set_Object(O->H_Root( ));
	O->renderable_Render( );
	::Render->set_Invisible(FALSE);
}

void CHUDManager::Render_Last( )
{
	if (!psHUD_Flags.is(HUD_WEAPON | HUD_WEAPON_RT))
	{
		return;
	}

	if (0 == pUI)
	{
		return;
	}

	CObject* O = g_pGameLevel->CurrentViewEntity( );
	if (0 == O)
	{
		return;
	}

	CActor* A = smart_cast<CActor*> (O);
	if (A && !A->HUDview( ))
	{
		return;
	}

	if (O->CLS_ID == CLSID_CAR)
	{
		return;
	}

	if (O->CLS_ID == CLSID_SPECTATOR)
	{
		return;
	}

	// hud itself
	::Render->set_HUD(TRUE);
	::Render->set_Object(O->H_Root( ));
	O->OnHUDDraw(this);
	::Render->set_HUD(FALSE);
}

extern void draw_wnds_rects( );
extern ENGINE_API BOOL bShowPauseString;
// ��������� ��������� ����������
void CHUDManager::RenderUI( )
{
	if (!b_online)
	{
		return;
	}

	BOOL bAlready = FALSE;
	if (true || psHUD_Flags.is(HUD_DRAW | HUD_DRAW_RT))
	{
		HitMarker.Render( );
		bAlready = !(pUI && !pUI->Render( ));
		Font( ).Render( );
	}

	if (psHUD_Flags.is(HUD_CROSSHAIR | HUD_CROSSHAIR_RT | HUD_CROSSHAIR_RT2) && !bAlready)
	{
		m_pHUDTarget->Render( );
	}

	draw_wnds_rects( );

	if (Device.Paused( ) && bShowPauseString)
	{
		CGameFont* pFont = Font( ).pFontGraffiti50Russian;
		pFont->SetColor(0x80FF0000);
		pcstr _str = CStringTable( ).translate("st_game_paused").c_str( );

		fVector2 _pos;
		_pos.set(UI_BASE_WIDTH / 2.0f, UI_BASE_HEIGHT / 2.0f);
		UI( )->ClientToScreenScaled(_pos);
		pFont->SetAligment(CGameFont::alCenter);
		pFont->Out(_pos.x, _pos.y, _str);
		pFont->OnRender( );
	}
}

void CHUDManager::OnEvent(EVENT E, u64 P1, u64 P2)
{ }

collide::rq_result& CHUDManager::GetCurrentRayQuery( )
{
	return m_pHUDTarget->RQ;
}

void CHUDManager::SetCrosshairDisp(f32 dispf, f32 disps)
{
	m_pHUDTarget->HUDCrosshair.SetDispersion(psHUD_Flags.test(HUD_CROSSHAIR_DYNAMIC) ? dispf : disps);
}

void CHUDManager::ShowCrosshair(bool show)
{
	m_pHUDTarget->m_bShowCrosshair = show;
}

void CHUDManager::Hit(s32 idx, f32 power, const fVector3& dir)
{
	HitMarker.Hit(idx, dir);
}

void CHUDManager::SetHitmarkType(pcstr tex_name)
{
	HitMarker.InitShader(tex_name);
}

void CHUDManager::OnScreenRatioChanged( )
{
	xr_delete(pUI->UIMainIngameWnd);

	pUI->UIMainIngameWnd = xr_new<CUIMainIngameWnd>( );
	pUI->UIMainIngameWnd->Init( );
	pUI->UnLoad( );
	pUI->Load(pUI->UIGame( ));
}

void CHUDManager::OnDisconnected( )
{
	b_online = false;
	if (pUI)
	{
		Device.seqFrame.Remove(pUI);
	}
}

void CHUDManager::OnConnected( )
{
	if (b_online)
	{
		return;
	}

	b_online = true;
	if (pUI)
	{
		Device.seqFrame.Add(pUI, REG_PRIORITY_LOW - 1000);
	}
}

void CHUDManager::net_Relcase(CObject* object)
{
	VERIFY(m_pHUDTarget);
	m_pHUDTarget->net_Relcase(object);
}
