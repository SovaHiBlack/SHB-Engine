#pragma once

#include "HUDCrosshair.h"//
#include "..\ENGINE\xr_collide_defs.h"

class CHUDManager;

class CHUDTarget
{
private:
	friend class CHUDManager;

private:
	using rq_result					= collide::rq_result;
	using rq_results				= collide::rq_results;

private:
	ref_shader						hShader;
	ref_geom						hGeom;
	float							fuzzyShowInfo;
	rq_result						RQ;
	rq_results						RQR;

private:
	bool							m_bShowCrosshair;
	CHUDCrosshair					HUDCrosshair;

private:
	void			net_Relcase		(CObject* O);

public:
					CHUDTarget		( );
	void			CursorOnFrame	( );
	void			Render			( );
	void			Load			( );
};
