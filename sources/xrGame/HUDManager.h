#pragma once

#include "..\ENGINE\CustomHUD.h"
#include "HitMarker.h"
#include "ui/UI.h"

class CContextMenu;
class CHUDTarget;

struct CFontManager : public pureDeviceReset
{
							CFontManager			( );
							~CFontManager			( );

	using FONTS_VEC									= xr_vector<CGameFont**>;
	using FONTS_VEC_IT								= FONTS_VEC::iterator;

	FONTS_VEC										m_all_fonts;
	void					Render					( );

	//hud font
	CGameFont*										pFontSmall;
	CGameFont*										pFontMedium;
	CGameFont*										pFontDI;
	CGameFont*										pFontArial14;
	CGameFont*										pFontGraffiti19Russian;
	CGameFont*										pFontGraffiti22Russian;
	CGameFont*										pFontGraffiti32Russian;
	CGameFont*										pFontGraffiti50Russian;
	CGameFont*										pFontLetterica16Russian;
	CGameFont*										pFontLetterica18Russian;
	CGameFont*										pFontLetterica25Russian;

	void					InitializeFonts			( );
	void					InitializeFont			(CGameFont*& F, const char* section, u32 flags = 0);
	const char*				GetFontTexName			(const char* section);

	virtual void			OnDeviceReset			( );
};

class CHUDManager : public CCustomHUD
{
	friend class CUI;

private:
	CUI*											pUI;
	CHitMarker										HitMarker;
	CHUDTarget*										m_pHUDTarget;
	bool											b_online;

public:
							CHUDManager				( );
	virtual					~CHUDManager			( );
	virtual void			OnEvent					(EVENT E, unsigned __int64 P1, unsigned __int64 P2);

	virtual void			Load					( );

	virtual void			Render_First			( );
	virtual void			Render_Last				( );
	virtual void			OnFrame					( );

	virtual void			RenderUI				( );

	virtual inline CUI*			GetUI					( )
	{
		return pUI;
	}

	void					Hit						(int idx, float power, const Fvector3& dir);
	CFontManager&			Font					( )
	{
		return *(UI( )->Font( ));
	}
	//текущий предмет на который смотрит HUD
	collide::rq_result&		GetCurrentRayQuery		( );

	//устанвка внешнего вида прицела в зависимости от текущей дисперсии
	void					SetCrosshairDisp		(float dispf, float disps = 0.0f);
	void					ShowCrosshair			(bool show);

	void					SetHitmarkType			(const char* tex_name);
	virtual void			OnScreenRatioChanged	( );
	virtual void			OnDisconnected			( );
	virtual void			OnConnected				( );
	virtual void			net_Relcase				(CObject* object);
};
