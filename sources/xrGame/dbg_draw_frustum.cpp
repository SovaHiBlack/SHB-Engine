#include "stdafx.h"
#include "Level.h"
#include "..\XR_3DA\frustum.h"

#ifdef DEBUG
#	include "DebugRenderer.h"
#endif

#ifdef DEBUG
void MK_Frustum(CFrustum& F, f32 FOV, f32 _FAR, f32 A, fVector3& P, fVector3& D, fVector3& U)
{
	f32 YFov = deg2rad(FOV);
	f32 XFov = deg2rad(FOV / A);

	// calc window extents in camera coords
	f32 wR = tanf(XFov * 0.5f);
	f32 wL = -wR;
	f32 wT = tanf(YFov * 0.5f);
	f32 wB = -wT;

	// calc x-axis (viewhoriz) and store cop
	// here we are assuring that vectors are perpendicular & normalized
	fVector3			R;
	fVector3	COP;
	D.normalize( );
	R.crossproduct(D, U);
	R.normalize( );
	U.crossproduct(R, D);
	U.normalize( );
	COP.set(P);

	// calculate the corner vertices of the window
	fVector3			sPts[4];  // silhouette points (corners of window)
	fVector3			Offset;
	fVector3	T;
	Offset.add(D, COP);

	sPts[0].mul(R, wR);	T.mad(Offset, U, wT);	sPts[0].add(T);
	sPts[1].mul(R, wL);	T.mad(Offset, U, wT);	sPts[1].add(T);
	sPts[2].mul(R, wL);	T.mad(Offset, U, wB);	sPts[2].add(T);
	sPts[3].mul(R, wR);	T.mad(Offset, U, wB);	sPts[3].add(T);

	// find projector direction vectors (from cop through silhouette pts)
	fVector3 ProjDirs[4];
	ProjDirs[0].sub(sPts[0], COP);
	ProjDirs[1].sub(sPts[1], COP);
	ProjDirs[2].sub(sPts[2], COP);
	ProjDirs[3].sub(sPts[3], COP);

	fVector3 _F[4];
	_F[0].mad(COP, ProjDirs[0], _FAR);
	_F[1].mad(COP, ProjDirs[1], _FAR);
	_F[2].mad(COP, ProjDirs[2], _FAR);
	_F[3].mad(COP, ProjDirs[3], _FAR);

	F.CreateFromPoints(_F, 4, COP);
}

void dbg_draw_frustum(f32 FOV, f32 _FAR, f32 A, fVector3& P, fVector3& D, fVector3& U)
{
	//if (!bDebug)		return;

	f32 YFov = deg2rad(FOV * A);
	f32 XFov = deg2rad(FOV);

	// calc window extents in camera coords
	f32 wR = tanf(XFov * 0.5f);
	f32 wL = -wR;
	f32 wT = tanf(YFov * 0.5f);
	f32 wB = -wT;

	// calc x-axis (viewhoriz) and store cop
	// here we are assuring that vectors are perpendicular & normalized
	fVector3			R;
	fVector3	COP;
	D.normalize( );
	R.crossproduct(D, U);
	R.normalize( );
	U.crossproduct(R, D);
	U.normalize( );
	COP.set(P);

	// calculate the corner vertices of the window
	fVector3			sPts[4];  // silhouette points (corners of window)
	fVector3			Offset;
	fVector3	T;
	Offset.add(D, COP);

	sPts[0].mul(R, wR);	T.mad(Offset, U, wT);	sPts[0].add(T);
	sPts[1].mul(R, wL);	T.mad(Offset, U, wT);	sPts[1].add(T);
	sPts[2].mul(R, wL);	T.mad(Offset, U, wB);	sPts[2].add(T);
	sPts[3].mul(R, wR);	T.mad(Offset, U, wB);	sPts[3].add(T);

	// find projector direction vectors (from cop through silhouette pts)
	fVector3 ProjDirs[4];
	ProjDirs[0].sub(sPts[0], COP);
	ProjDirs[1].sub(sPts[1], COP);
	ProjDirs[2].sub(sPts[2], COP);
	ProjDirs[3].sub(sPts[3], COP);

	RCache.set_CullMode(CULL_NONE);
	CHK_DX(HW.pDevice->SetRenderState(D3DRS_AMBIENT, 0xffffffff));

	fVector3 _F[4];
	_F[0].mad(COP, ProjDirs[0], _FAR);
	_F[1].mad(COP, ProjDirs[1], _FAR);
	_F[2].mad(COP, ProjDirs[2], _FAR);
	_F[3].mad(COP, ProjDirs[3], _FAR);

	//	u32 CT	= color_rgba(255,255,255,64);
	u32 CL = color_rgba(255, 0, 0, 255);
	fMatrix4x4& M = Fidentity;
	ref_shader				l_tShaderReference = Level( ).ObjectSpace.dbgGetShader( );
	RCache.set_Shader(l_tShaderReference);
	//	RCache.dbg_DrawTRI	(M,COP,_F[0],_F[1],CT);
	//	RCache.dbg_DrawTRI	(M,COP,_F[1],_F[2],CT);
	//	RCache.dbg_DrawTRI	(M,COP,_F[2],_F[3],CT);
	//	RCache.dbg_DrawTRI	(M,COP,_F[3],_F[0],CT);
	Level( ).debug_renderer( ).draw_line(M, COP, _F[0], CL);
	Level( ).debug_renderer( ).draw_line(M, COP, _F[1], CL);
	Level( ).debug_renderer( ).draw_line(M, COP, _F[2], CL);
	Level( ).debug_renderer( ).draw_line(M, COP, _F[3], CL);

	Level( ).debug_renderer( ).draw_line(M, _F[0], _F[1], CL);
	Level( ).debug_renderer( ).draw_line(M, _F[1], _F[2], CL);
	Level( ).debug_renderer( ).draw_line(M, _F[2], _F[3], CL);
	Level( ).debug_renderer( ).draw_line(M, _F[3], _F[0], CL);

	RCache.set_CullMode(CULL_CCW);
	CHK_DX(HW.pDevice->SetRenderState(D3DRS_AMBIENT, 0));
}
#endif
