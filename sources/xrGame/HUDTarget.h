#pragma once

#include "HUDCrosshair.h"//
#include "..\ENGINE\xr_collide_defs.h"

class CHUDManager;

class CHUDTarget
{
private:
	friend class CHUDManager;

	using rq_result						= collide::rq_result;
	using rq_results					= collide::rq_results;

	ref_shader							hShader;
	ref_geom							hGeom;
	float								fuzzyShowInfo;
	rq_result							RQ;
	rq_results							RQR;

	bool								m_bShowCrosshair;
	CHUDCrosshair						HUDCrosshair;

	void				net_Relcase		(CObject* O);

public:
						CHUDTarget		( );
	void				CursorOnFrame	( );
	void				Render			( );
	void				Load			( );
};
