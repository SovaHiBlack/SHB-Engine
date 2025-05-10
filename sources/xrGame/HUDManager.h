#pragma once

#include "..\XR_3DA\CustomHUD.h"
#include "HitMarker.h"
#include "UI.h"

class CContextMenu;
class CHUDTarget;

struct CFontManager : public pureDeviceReset
{
	CFontManager( );
	~CFontManager( );

	typedef xr_vector<CGameFont**>					FONTS_VEC;
	typedef FONTS_VEC::iterator						FONTS_VEC_IT;
	FONTS_VEC				m_all_fonts;
	void					Render( );

	// hud font
	CGameFont* pFontSmall;
	CGameFont* pFontMedium;
	CGameFont* pFontDI;
	CGameFont* pFontArial14;
	CGameFont* pFontArial21;
	CGameFont* pFontGraffiti19Russian;
	CGameFont* pFontGraffiti22Russian;
	CGameFont* pFontGraffiti32Russian;
	CGameFont* pFontGraffiti40Russian;
	CGameFont* pFontGraffiti50Russian;
	CGameFont* pFontLetterica16Russian;
	CGameFont* pFontLetterica18Russian;
	CGameFont* pFontLetterica25Russian;

	void					InitializeFonts( );
	void					InitializeFont(CGameFont*& F, pcstr section, u32 flags = 0);
	pcstr					GetFontTexName(pcstr section);

	virtual void			OnDeviceReset( );
};

class CHUDManager : public CCustomHUD
{
	friend class CUI;

private:
	CUI* pUI;
	CHitMarker				HitMarker;
	CHUDTarget* m_pHUDTarget;
	bool					b_online;

public:
	CHUDManager( );
	virtual					~CHUDManager( );
	virtual void		OnEvent(EVENT E, u64 P1, u64 P2);

	virtual void		Load( );

	virtual void		Render_First( );
	virtual void		Render_Last( );
	virtual void		OnFrame( );

	virtual void		RenderUI( );

	virtual IC CUI* GetUI( )
	{
		return pUI;
	}

	void		Hit(s32 idx, f32 power, const fVector3& dir);
	CFontManager& Font( )
	{
		return *(UI( )->Font( ));
	}

	//������� ������� �� ������� ������� HUD
	collide::rq_result& GetCurrentRayQuery( );

	//�������� �������� ���� ������� � ����������� �� ������� ���������
	void					SetCrosshairDisp(f32 dispf, f32 disps = 0.0f);
	void					ShowCrosshair(bool show);

	void					SetHitmarkType(pcstr tex_name);
	virtual void			OnScreenRatioChanged( );
	virtual void			OnDisconnected( );
	virtual void			OnConnected( );
	virtual void			net_Relcase(CObject* object);
};
