#include "stdafx.h"
#include "..\XR_3DA\cl_intersect.h"

extern fVector3 du_cone_vertices			[DU_CONE_NUMVERTEX];

BOOL	tri_vs_sphere_intersect			(fVector3& SC, f32 R, fVector3& v0, fVector3& v1, fVector3& v2)
{
	fVector3	e0;
	fVector3	e1;
	return	CDB::TestSphereTri	(SC,R,v0,e0.sub(v1,v0),e1.sub(v2,v0));
}

void CRenderTarget::enable_dbt_bounds		(light* L)
{
	if (!RImplementation.o.nvdbt)					return;
	if (!ps_r2_ls_flags.test(R2FLAG_USE_NVDBT))		return;

	u32	mask		= 0xffffffff;
	EFC_Visible vis	= RImplementation.ViewBase.testSphere(L->spatial.sphere.P,L->spatial.sphere.R*1.01f,mask);
	if (vis!=fcvFully)								return;

	// xform BB
	fBox3	BB;
	fVector3	rr;
	rr.set(L->spatial.sphere.R,L->spatial.sphere.R,L->spatial.sphere.R);
	BB.setb	(L->spatial.sphere.P, rr);

	fBox3	bbp;
	bbp.invalidate();
	for (u32 i=0; i<8; i++)		{
		fVector3		pt;
		BB.getpoint	(i,pt);
		Device.mFullTransform.transform	(pt);
		bbp.modify	(pt);
	}
	u_DBT_enable	(bbp.min.z,bbp.max.z);
}

// nv-DBT
BOOL	CRenderTarget::u_DBT_enable	(f32 zMin, f32 zMax)
{
	if (!RImplementation.o.nvdbt)					return	FALSE;
	if (!ps_r2_ls_flags.test(R2FLAG_USE_NVDBT))		return	FALSE;

	// enable cheat
	HW.pDevice->SetRenderState(D3DRS_ADAPTIVETESS_X,MAKEFOURCC('N','V','D','B'));
	HW.pDevice->SetRenderState(D3DRS_ADAPTIVETESS_Z,*(DWORD*)&zMin);
	HW.pDevice->SetRenderState(D3DRS_ADAPTIVETESS_W,*(DWORD*)&zMax); 

	return TRUE;
}

void	CRenderTarget::u_DBT_disable	()
{
	if (RImplementation.o.nvdbt && ps_r2_ls_flags.test(R2FLAG_USE_NVDBT))	
		HW.pDevice->SetRenderState(D3DRS_ADAPTIVETESS_X,0);
}

BOOL CRenderTarget::enable_scissor		(light* L)		// true if intersects near plane
{
	// Msg	("%d: %x type(%d), pos(%f,%f,%f)",Device.dwFrame,u32(L),u32(L->flags.type),VPUSH(L->position));

	// Near plane intersection
	BOOL	near_intersect				= FALSE;
	{
		fMatrix4x4& M						= Device.mFullTransform;
		fVector4 plane;
		plane.x							= -(M._14 + M._13);
		plane.y							= -(M._24 + M._23);
		plane.z							= -(M._34 + M._33);
		plane.w							= -(M._44 + M._43);
		f32 denom						= -1.0f / _sqrt(_sqr(plane.x)+_sqr(plane.y)+_sqr(plane.z));
		plane.mul						(denom);
		fPlane3							P;
		P.n.set							(plane.x,plane.y,plane.z);
		P.d								= plane.w;
		f32	p_dist						= P.classify	(L->spatial.sphere.P) - L->spatial.sphere.R;
		near_intersect					= (p_dist<=0);
	}

#ifdef DEBUG
	if (1)
	{
		fSphere		S;
		S.set	(L->spatial.sphere.P,L->spatial.sphere.R);
		dbg_spheres.push_back	(mk_pair(S,L->color));
	}
#endif

	// Scissor
	//. disable scissor because some bugs prevent it to work through multi-portals
	//. if (!HW.Caps.bScissor)	return		near_intersect;
	return		near_intersect;

#if 0
	CSector*	S	= (CSector*)L->spatial.sector;
	_scissor	bb	= S->r_scissor_merged;
	iRect		R;
	R.x1		= clampr	(iFloor	(bb.min.x*Device.dwWidth),	int(0),int(Device.dwWidth));
	R.x2		= clampr	(iCeil	(bb.max.x*Device.dwWidth),	int(0),int(Device.dwWidth));
	R.y1		= clampr	(iFloor	(bb.min.y*Device.dwHeight),	int(0),int(Device.dwHeight));
	R.y2		= clampr	(iCeil	(bb.max.y*Device.dwHeight),	int(0),int(Device.dwHeight));
	if	( (Device.dwWidth==u32(R.right - R.left)) && (Device.dwHeight==u32(R.bottom-R.top)) )
	{
		// full-screen -> do nothing
	} else {
		// test if light-volume intersects scissor-rectangle
		// if it does - do nothing, if doesn't - we look on light through portal

		// 1. convert rect into -1..1 space
		fBox2		b_pp	= bb;
		b_pp.min.x			= b_pp.min.x * 2 - 1;
		b_pp.max.x			= b_pp.max.x * 2 - 1;
		b_pp.min.y			= (1-b_pp.min.y) * 2 - 1;
		b_pp.max.y			= (1-b_pp.max.y) * 2 - 1;

		// 2. construct scissor rectangle in post-projection space
		fVector3	s_points_pp			[4];
		s_points_pp[0].set	(bb.min.x,bb.min.y,bb.depth);	// LT
		s_points_pp[1].set	(bb.max.x,bb.min.y,bb.depth);	// RT
		s_points_pp[2].set	(bb.max.x,bb.max.y,bb.depth);	// RB
		s_points_pp[3].set	(bb.min.x,bb.max.y,bb.depth);	// LB

		// 3. convert it into world space
		fVector3	s_points			[4];
		fMatrix4x4&	iVP					= Device.mInvFullTransform;
		iVP.transform	(s_points[0],s_points_pp[0]);
		iVP.transform	(s_points[1],s_points_pp[1]);
		iVP.transform	(s_points[2],s_points_pp[2]);
		iVP.transform	(s_points[3],s_points_pp[3]);

		// 4. check intersection of two triangles with (spatial, enclosing) sphere
		BOOL	bIntersect	= tri_vs_sphere_intersect	(
			L->spatial.sphere.P,L->spatial.sphere.R,
			s_points[0],s_points[1],s_points[2]
			);
		if (!bIntersect)	bIntersect	= tri_vs_sphere_intersect	(
				L->spatial.sphere.P,L->spatial.sphere.R,
				s_points[2],s_points[3],s_points[0]
				);
		if (!bIntersect)	{
			// volume doesn't touch scissor - enable mask
			RCache.set_Scissor(&R);
			//CHK_DX		(HW.pDevice->SetRenderState(D3DRS_SCISSORTESTENABLE,TRUE));
			//CHK_DX		(HW.pDevice->SetScissorRect(&R));
		} else {
			// __asm int 3;
			RCache.set_Scissor(NULL);
		}
	}

	return near_intersect;
#endif
}
/*
{
	fMatrix4x4& M						= RCache.xforms.m_wvp;
	BOOL	bIntersect				= FALSE;
	for (u32 vit=0; vit<DU_CONE_NUMVERTEX; vit++)	{
		fVector3&	v	= du_cone_vertices[vit];
		f32 _z = v.x*M._13 + v.y*M._23 + v.z*M._33 + M._43;
		f32 _w = v.x*M._14 + v.y*M._24 + v.z*M._34 + M._44;
		if (_z<=0 || _w<=0)	{
			bIntersect	= TRUE;
			break;
		}
	}
}
*/
