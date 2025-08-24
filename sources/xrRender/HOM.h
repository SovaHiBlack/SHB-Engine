// HOM.h: interface for the CHOM class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include "..\XR_3DA\IGamePersistent.h"

class occTri;

class CHOM  
#ifdef DEBUG
	: public pureRender
#endif
{
private:
	xrXRC					xrc;
	CDB::MODEL*				m_pModel;
	occTri*					m_pTris;
	BOOL					bEnabled;
	fMatrix4x4					m_xform;
	fMatrix4x4					m_xform_01;

#ifdef DEBUG
	u32						tris_in_frame_visible	;
	u32						tris_in_frame			;
#endif

	xrCriticalSection		MT;
	volatile u32			MT_frame_rendered;

	void					Render_DB	(CFrustum&	base);
public:
	void					Load		();
	void					Unload		();
	void					Render		(CFrustum&	base);

	void					occlude		(fBox2&		space) { }
	void					Disable		();
	void					Enable		();

	void	__stdcall		MT_RENDER	();
	ICF	void				MT_SYNC		()			{ 
		if (g_pGamePersistent->m_pMainMenu && g_pGamePersistent->m_pMainMenu->IsActive())
			return;

		MT_RENDER			(); 
	}

	BOOL					visible		(vis_data&	vis);
	BOOL					visible		(fBox3&		B);
	BOOL					visible		(sPoly&		P);
	BOOL					visible		(fBox2&		B, f32 depth);	// viewport-space (0..1)

	CHOM	();
	~CHOM	();

#ifdef DEBUG
	virtual void			OnRender	();
			void			stats		();
#endif
};
