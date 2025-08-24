#include "stdafx.h"
#include "..\XR_3DA\CustomHUD.h"

f32				g_fSCREEN;

extern f32		r_dtex_range;
extern f32		r_ssaDISCARD;
extern f32		r_ssaDONTSORT;
extern f32		r_ssaLOD_A;
extern f32		r_ssaLOD_B;
extern f32		r_ssaHZBvsTEX;
extern f32		r_ssaGLOD_start;
extern f32		r_ssaGLOD_end;

void CRender::Calculate( )
{
	// Transfer to global space to avoid deep pointer access
	IRender_Target* T = getTarget( );
	f32	fov_factor = _sqr(90.f / Device.fFOV);
	g_fSCREEN = f32(T->get_width( ) * T->get_height( )) * fov_factor * (EPSILON_7 + ps_r__LOD);
	r_ssaDISCARD = _sqr(ps_r__ssaDISCARD) / g_fSCREEN;
	r_ssaDONTSORT = _sqr(ps_r__ssaDONTSORT / 3) / g_fSCREEN;
	r_ssaLOD_A = _sqr(ps_r2_ssaLOD_A / 3) / g_fSCREEN;
	r_ssaLOD_B = _sqr(ps_r2_ssaLOD_B / 3) / g_fSCREEN;
	r_ssaGLOD_start = _sqr(ps_r__GLOD_ssa_start / 3) / g_fSCREEN;
	r_ssaGLOD_end = _sqr(ps_r__GLOD_ssa_end / 3) / g_fSCREEN;
	r_ssaHZBvsTEX = _sqr(ps_r__ssaHZBvsTEX / 3) / g_fSCREEN;
	r_dtex_range = ps_r2_df_parallax_range * g_fSCREEN / (1024.f * 768.f);

	// Detect camera-sector
	if (!vLastCameraPos.similar(Device.vCameraPosition, EPSILON_7))
	{
		CSector* pSector = (CSector*)detectSector(Device.vCameraPosition);
		if (0 == pSector) pSector = pLastSector;
		pLastSector = pSector;
		vLastCameraPos.set(Device.vCameraPosition);
	}

	// Check if camera is too near to some portal - if so force DualRender
	if (rmPortals)
	{
		f32	eps = VIEWPORT_NEAR + EPSILON_3;
		fVector3 box_radius;
		box_radius.set(eps, eps, eps);
		Sectors_xrc.box_options(CDB::OPT_FULL_TEST);
		Sectors_xrc.box_query(rmPortals, Device.vCameraPosition, box_radius);
		for (int K = 0; K < Sectors_xrc.r_count( ); K++)
		{
			CPortal* pPortal = (CPortal*)Portals[rmPortals->get_tris( )[Sectors_xrc.r_begin( )[K].id].dummy];
			pPortal->bDualRender = TRUE;
		}
	}

	//
	Lights.Update( );

	// Check if we touch some light even trough portal
	lstRenderables.clear( );
	g_SpatialSpace->q_sphere(lstRenderables, 0, STYPE_LIGHTSOURCE, Device.vCameraPosition, EPSILON_3);
	for (u32 _it = 0; _it < lstRenderables.size( ); _it++)
	{
		ISpatial* spatial = lstRenderables[_it];		spatial->spatial_updatesector( );
		CSector* sector = (CSector*)spatial->spatial.sector;
		if (0 == sector)										continue;	// disassociated from S/P structure

		VERIFY(spatial->spatial.type & STYPE_LIGHTSOURCE);
		// lightsource
		light* L = (light*)(spatial->dcast_Light( ));
		VERIFY(L);
		Lights.add_light(L);
	}
}
