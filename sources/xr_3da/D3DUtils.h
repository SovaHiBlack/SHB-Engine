//----------------------------------------------------
// file: D3DUtils.h
//----------------------------------------------------
#pragma once

#include "DrawUtils.h"
//----------------------------------------------------

#define DU_DRAW_DIP	RCache.dbg_DIP
#define DU_DRAW_DP	RCache.dbg_DP

struct SPrimitiveBuffer
{
	ref_geom				pGeom;
	u32						v_cnt;
	u32						i_cnt;
	D3DPRIMITIVETYPE 		p_type;
	u32						p_cnt;
	typedef fastdelegate::FastDelegate0<> TOnRender;
	TOnRender				OnRender;
	void xr_stdcall			RenderDIP( )
	{
		DU_DRAW_DIP(p_type, pGeom, 0, 0, v_cnt, 0, p_cnt);
	}
	void xr_stdcall			RenderDP( )
	{
		DU_DRAW_DP(p_type, pGeom, 0, p_cnt);
	}

public:
	SPrimitiveBuffer( ) : OnRender(0), pGeom(0)
	{ }
	void					CreateFromData(D3DPRIMITIVETYPE _pt, u32 _p_cnt, u32 FVF, LPVOID vertices, u32 _v_cnt, u16* indices = 0, u32 _i_cnt = 0);
	void					Destroy( );
	void					Render( )
	{
		OnRender( );
	}
};

//----------------------------------------------------
// Utilities
//----------------------------------------------------
class CDrawUtilities : public CDUInterface, public pureRender
{
	SPrimitiveBuffer		m_SolidCone;
	SPrimitiveBuffer		m_WireCone;
	SPrimitiveBuffer		m_SolidSphere;
	SPrimitiveBuffer		m_WireSphere;
	SPrimitiveBuffer		m_SolidSpherePart;
	SPrimitiveBuffer		m_WireSpherePart;
	SPrimitiveBuffer		m_SolidCylinder;
	SPrimitiveBuffer		m_WireCylinder;
	SPrimitiveBuffer		m_SolidBox;
	SPrimitiveBuffer		m_WireBox;
	CGameFont* m_Font;

public:
	ref_geom 	vs_L;
	ref_geom 	vs_TL;
	ref_geom 	vs_LIT;

protected:
	FVF::L* m_DD_pv;
	FVF::L* m_DD_pv_start;
	u32 		m_DD_base;
	BOOL		m_DD_wire;
	void 		DD_DrawFace_flush(BOOL try_again);

public:
	void 		DD_DrawFace_begin(BOOL bWire);
	void 		DD_DrawFace_push(const fVector3& p0, const fVector3& p1, const fVector3& p2, u32 clr);
	void 		DD_DrawFace_end( );

public:
	CDrawUtilities( )
	{
		vs_L = 0;
		vs_TL = 0;
		vs_LIT = 0;
		m_DD_pv = 0;
		m_DD_pv_start = 0;
		m_DD_base = 0;
		m_Font = 0;
	}

	void OnDeviceCreate( );
	void OnDeviceDestroy( );

	void UpdateGrid(s32 number_of_cell, f32 square_size, s32 subdiv = 10);

	//----------------------------------------------------
	virtual void __stdcall DrawCross(const fVector3& p, f32 szx1, f32 szy1, f32 szz1, f32 szx2, f32 szy2, f32 szz2, u32 clr, BOOL bRot45 = false);
	virtual void __stdcall DrawCross(const fVector3& p, f32 sz, u32 clr, BOOL bRot45 = false)
	{
		DrawCross(p, sz, sz, sz, sz, sz, sz, clr, bRot45);
	}
	virtual void __stdcall DrawEntity(u32 clr, ref_shader s);
	virtual void __stdcall DrawFlag(const fVector3& p, f32 heading, f32 height, f32 sz, f32 sz_fl, u32 clr, BOOL bDrawEntity);
	virtual void __stdcall DrawRomboid(const fVector3& p, f32 radius, u32 clr);
	virtual void __stdcall DrawJoint(const fVector3& p, f32 radius, u32 clr);

	virtual void __stdcall DrawSpotLight(const fVector3& p, const fVector3& d, f32 range, f32 phi, u32 clr);
	virtual void __stdcall DrawDirectionalLight(const fVector3& p, const fVector3& d, f32 radius, f32 range, u32 clr);
	virtual void __stdcall DrawPointLight(const fVector3& p, f32 radius, u32 clr);

	virtual void __stdcall DrawSound(const fVector3& p, f32 radius, u32 clr);
	virtual void __stdcall DrawLineSphere(const fVector3& p, f32 radius, u32 clr, BOOL bCross);

	virtual void __stdcall dbgDrawPlacement(const fVector3& p, s32 sz, u32 clr, pcstr caption = 0, u32 clr_font = 0xffffffff);
	virtual void __stdcall dbgDrawVert(const fVector3& p0, u32 clr, pcstr caption = 0);
	virtual void __stdcall dbgDrawEdge(const fVector3& p0, const fVector3& p1, u32 clr, pcstr caption = 0);
	virtual void __stdcall dbgDrawFace(const fVector3& p0, const fVector3& p1, const fVector3& p2, u32 clr, pcstr caption = 0);

	virtual void __stdcall DrawFace(const fVector3& p0, const fVector3& p1, const fVector3& p2, u32 clr_s, u32 clr_w, BOOL bSolid, BOOL bWire);
	virtual void __stdcall DrawLine(const fVector3& p0, const fVector3& p1, u32 clr);
	IC virtual void __stdcall DrawLine(const fVector3* p, u32 clr)
	{
		DrawLine(p[0], p[1], clr);
	}
	virtual void __stdcall DrawLink(const fVector3& p0, const fVector3& p1, f32 sz, u32 clr);
	IC virtual void __stdcall DrawFaceNormal(const fVector3& p0, const fVector3& p1, const fVector3& p2, f32 size, u32 clr)
	{
		fVector3 N;
		fVector3 C;
		fVector3 P;
		N.mknormal(p0, p1, p2);
		C.set(p0);
		C.add(p1);
		C.add(p2);
		C.div(3);
		P.mad(C, N, size);
		DrawLine(C, P, clr);
	}
	IC virtual void __stdcall DrawFaceNormal(const fVector3* p, f32 size, u32 clr)
	{
		DrawFaceNormal(p[0], p[1], p[2], size, clr);
	}
	IC virtual void __stdcall DrawFaceNormal(const fVector3& C, const fVector3& N, f32 size, u32 clr)
	{
		fVector3 P;
		P.mad(C, N, size);
		DrawLine(C, P, clr);
	}
	virtual void __stdcall DrawSelectionBox(const fVector3& center, const fVector3& size, u32* c = 0);
	IC virtual void __stdcall DrawSelectionBox(const fBox3& box, u32* c = 0)
	{
		fVector3 S;
		fVector3 C;
		box.getsize(S);
		box.getcenter(C);
		DrawSelectionBox(C, S, c);
	}
	virtual void __stdcall DrawIdentSphere(BOOL bSolid, BOOL bWire, u32 clr_s, u32 clr_w);
	virtual void __stdcall DrawIdentSpherePart(BOOL bSolid, BOOL bWire, u32 clr_s, u32 clr_w);
	virtual void __stdcall DrawIdentCone(BOOL bSolid, BOOL bWire, u32 clr_s, u32 clr_w);
	virtual void __stdcall DrawIdentCylinder(BOOL bSolid, BOOL bWire, u32 clr_s, u32 clr_w);
	virtual void __stdcall DrawIdentBox(BOOL bSolid, BOOL bWire, u32 clr_s, u32 clr_w);

	virtual void __stdcall DrawBox(const fVector3& offs, const fVector3& Size, BOOL bSolid, BOOL bWire, u32 clr_s, u32 clr_w);
	virtual void __stdcall DrawAABB(const fVector3& p0, const fVector3& p1, u32 clr_s, u32 clr_w, BOOL bSolid, BOOL bWire);
	virtual void __stdcall DrawAABB(const fMatrix4x4& parent, const fVector3& center, const fVector3& size, u32 clr_s, u32 clr_w, BOOL bSolid, BOOL bWire);
	virtual void __stdcall DrawOBB(const fMatrix4x4& parent, const fObb& box, u32 clr_s, u32 clr_w);
	virtual void __stdcall DrawSphere(const fMatrix4x4& parent, const fVector3& center, f32 radius, u32 clr_s, u32 clr_w, BOOL bSolid, BOOL bWire);
	virtual void __stdcall DrawSphere(const fMatrix4x4& parent, const fSphere& S, u32 clr_s, u32 clr_w, BOOL bSolid, BOOL bWire)
	{
		DrawSphere(parent, S.P, S.R, clr_s, clr_w, bSolid, bWire);
	}
	virtual void __stdcall DrawCylinder(const fMatrix4x4& parent, const fVector3& center, const fVector3& dir, f32 height, f32 radius, u32 clr_s, u32 clr_w, BOOL bSolid, BOOL bWire);
	virtual void __stdcall DrawCone(const fMatrix4x4& parent, const fVector3& apex, const fVector3& dir, f32 height, f32 radius, u32 clr_s, u32 clr_w, BOOL bSolid, BOOL bWire);
	virtual void __stdcall DrawPlane(const fVector3& center, const fVector2& scale, const fVector3& rotate, u32 clr_s, u32 clr_w, BOOL bCull, BOOL bSolid, BOOL bWire);
	virtual void __stdcall DrawPlane(const fVector3& p, const fVector3& n, const fVector2& scale, u32 clr_s, u32 clr_w, BOOL bCull, BOOL bSolid, BOOL bWire);
	virtual void __stdcall DrawRectangle(const fVector3& o, const fVector3& u, const fVector3& v, u32 clr_s, u32 clr_w, BOOL bSolid, BOOL bWire);

	virtual void __stdcall DrawGrid( );
	virtual void __stdcall DrawPivot(const fVector3& pos, f32 sz = 5.0f);
	virtual void __stdcall DrawAxis(const fMatrix4x4& T);
	virtual void __stdcall DrawObjectAxis(const fMatrix4x4& T, f32 sz, BOOL sel);
	virtual void __stdcall DrawSelectionRect(const iVector2& m_SelStart, const iVector2& m_SelEnd);

	virtual void __stdcall DrawPrimitiveL(D3DPRIMITIVETYPE pt, u32 pc, fVector3* vertices, s32 vc, u32 color, BOOL bCull, BOOL bCycle);
	virtual void __stdcall DrawPrimitiveTL(D3DPRIMITIVETYPE pt, u32 pc, FVF::TL* vertices, s32 vc, BOOL bCull, BOOL bCycle);
	virtual void __stdcall DrawPrimitiveLIT(D3DPRIMITIVETYPE pt, u32 pc, FVF::LIT* vertices, s32 vc, BOOL bCull, BOOL bCycle);

	virtual void __stdcall OutText(const fVector3& pos, pcstr text, u32 color = 0xFF000000, u32 shadow_color = 0xFF909090);

	virtual void 			OnRender( );
};
extern CDrawUtilities DU;
