//----------------------------------------------------
// file: D3DUtils.h
//----------------------------------------------------
#pragma once

#include "DrawUtils.h"
//----------------------------------------------------

#define DU_DRAW_DIP	RCache.dbg_DIP
#define DU_DRAW_DP	RCache.dbg_DP

struct SPrimitiveBuffer{
	ref_geom				pGeom;
	unsigned int						v_cnt;
	unsigned int						i_cnt;
	D3DPRIMITIVETYPE 		p_type;
	unsigned int						p_cnt;
	typedef fastdelegate::FastDelegate0<> TOnRender;
	TOnRender				OnRender;
	void xr_stdcall			RenderDIP()	{DU_DRAW_DIP(p_type,pGeom,0,0,v_cnt,0,p_cnt);}
	void xr_stdcall			RenderDP()	{DU_DRAW_DP	(p_type,pGeom,0,p_cnt);}

public:
							SPrimitiveBuffer():OnRender(0),pGeom(0){;}
	void					CreateFromData(D3DPRIMITIVETYPE _pt, unsigned int _p_cnt, unsigned int FVF, void* vertices, unsigned int _v_cnt, unsigned short* indices=0, unsigned int _i_cnt=0);
	void					Destroy();
	void					Render(){OnRender();}
};

//----------------------------------------------------
// Utilities
//----------------------------------------------------
class CDrawUtilities: public CDUInterface, public pureRender{
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
	CGameFont* 				m_Font;
public:
	ref_geom 	vs_L;
	ref_geom 	vs_TL;
	ref_geom 	vs_LIT;
protected:
	FVF::L*		m_DD_pv;
	FVF::L*		m_DD_pv_start;
	unsigned int 		m_DD_base;
	BOOL		m_DD_wire;
	void 		DD_DrawFace_flush		(BOOL try_again);
public:
	void 		DD_DrawFace_begin		(BOOL bWire);
	void 		DD_DrawFace_push		(const Fvector3& p0, const Fvector3& p1, const Fvector3& p2, unsigned int clr);
	void 		DD_DrawFace_end			();
public:
	CDrawUtilities()
	{
		vs_L	= 0;
		vs_TL	= 0;
		vs_LIT	= 0;
		m_DD_pv			= 0;
		m_DD_pv_start	= 0;
		m_DD_base		= 0;
		m_Font	= 0;
	}
	
	void OnDeviceCreate	();
	void OnDeviceDestroy();

	void UpdateGrid(int number_of_cell, float square_size, int subdiv=10);

	//----------------------------------------------------
	virtual void __stdcall DrawCross(const Fvector3& p, float szx1, float szy1, float szz1, float szx2, float szy2, float szz2, unsigned int clr, BOOL bRot45=false);
	virtual void __stdcall DrawCross(const Fvector3& p, float sz, unsigned int clr, BOOL bRot45=false){ DrawCross(p, sz,sz,sz, sz,sz,sz, clr, bRot45); }
	virtual void __stdcall DrawEntity(unsigned int clr, ref_shader s);
	virtual void __stdcall DrawFlag(const Fvector3& p, float heading, float height, float sz, float sz_fl, unsigned int clr, BOOL bDrawEntity);
	virtual void __stdcall DrawRomboid(const Fvector3& p, float radius, unsigned int clr);
	virtual void __stdcall DrawJoint(const Fvector3& p, float radius, unsigned int clr);

	virtual void __stdcall DrawSpotLight(const Fvector3& p, const Fvector3& d, float range, float phi, unsigned int clr);
	virtual void __stdcall DrawDirectionalLight(const Fvector3& p, const Fvector3& d, float radius, float range, unsigned int clr);
	virtual void __stdcall DrawPointLight(const Fvector3& p, float radius, unsigned int clr);

	virtual void __stdcall DrawSound(const Fvector3& p, float radius, unsigned int clr);
	virtual void __stdcall DrawLineSphere(const Fvector3& p, float radius, unsigned int clr, BOOL bCross);

	virtual void __stdcall dbgDrawPlacement(const Fvector3& p, int sz, unsigned int clr, const char* caption=0, unsigned int clr_font=0xffffffff);
	virtual void __stdcall dbgDrawVert(const Fvector3& p0, unsigned int clr, const char* caption=0);
	virtual void __stdcall dbgDrawEdge(const Fvector3& p0,	const Fvector3& p1, unsigned int clr, const char* caption=0);
	virtual void __stdcall dbgDrawFace(const Fvector3& p0,	const Fvector3& p1, const Fvector3& p2, unsigned int clr, const char* caption=0);

	virtual void __stdcall DrawFace(const Fvector3& p0,	const Fvector3& p1, const Fvector3& p2, unsigned int clr_s, unsigned int clr_w, BOOL bSolid, BOOL bWire);
	virtual void __stdcall DrawLine(const Fvector3& p0,	const Fvector3& p1, unsigned int clr);
	inline virtual void __stdcall DrawLine(const Fvector3* p, unsigned int clr){DrawLine(p[0],p[1],clr);}
	virtual void __stdcall DrawLink(const Fvector3& p0, const Fvector3& p1, float sz, unsigned int clr);
	inline virtual void __stdcall DrawFaceNormal(const Fvector3& p0, const Fvector3& p1, const Fvector3& p2, float size, unsigned int clr){
		Fvector3 N;
		Fvector3 C;
		Fvector3 P;
		N.mknormal(p0, p1, p2);
		C.set(p0);
		C.add(p1);
		C.add(p2);
		C.div(3);
		P.mad(C,N,size);
		DrawLine(C,P,clr);}
	inline virtual void __stdcall DrawFaceNormal(const Fvector3* p, float size, unsigned int clr){DrawFaceNormal(p[0],p[1],p[2],size,clr);}
	inline virtual void __stdcall DrawFaceNormal(const Fvector3& C, const Fvector3& N, float size, unsigned int clr){
		Fvector3 P;
		P.mad(C,N,size);
		DrawLine(C,P,clr);}
	virtual void __stdcall DrawSelectionBox(const Fvector3& center, const Fvector3& size, unsigned int* c=0);
	inline virtual void __stdcall DrawSelectionBox(const Fbox3& box, unsigned int* c=0){
		Fvector3 S;
		Fvector3 C;
		box.getsize(S);
		box.getcenter(C);
		DrawSelectionBox(C,S,c);
	}
	virtual void __stdcall DrawIdentSphere	(BOOL bSolid, BOOL bWire, unsigned int clr_s, unsigned int clr_w);
	virtual void __stdcall DrawIdentSpherePart(BOOL bSolid, BOOL bWire, unsigned int clr_s, unsigned int clr_w);
	virtual void __stdcall DrawIdentCone		(BOOL bSolid, BOOL bWire, unsigned int clr_s, unsigned int clr_w);
	virtual void __stdcall DrawIdentCylinder	(BOOL bSolid, BOOL bWire, unsigned int clr_s, unsigned int clr_w);
	virtual void __stdcall DrawIdentBox		(BOOL bSolid, BOOL bWire, unsigned int clr_s, unsigned int clr_w);

	virtual void __stdcall DrawBox	(const Fvector3& offs, const Fvector3& Size, BOOL bSolid, BOOL bWire, unsigned int clr_s, unsigned int clr_w);
	virtual void __stdcall DrawAABB	(const Fvector3& p0, const Fvector3& p1, unsigned int clr_s, unsigned int clr_w, BOOL bSolid, BOOL bWire);
	virtual void __stdcall DrawAABB	(const Fmatrix& parent, const Fvector3& center, const Fvector3& size, unsigned int clr_s, unsigned int clr_w, BOOL bSolid, BOOL bWire);
	virtual void __stdcall DrawOBB	(const Fmatrix& parent, const Fobb& box, unsigned int clr_s, unsigned int clr_w);
	virtual void __stdcall DrawSphere	(const Fmatrix& parent, const Fvector3& center, float radius, unsigned int clr_s, unsigned int clr_w, BOOL bSolid, BOOL bWire);
	virtual void __stdcall DrawSphere	(const Fmatrix& parent, const Fsphere& S, unsigned int clr_s, unsigned int clr_w, BOOL bSolid, BOOL bWire){DrawSphere(parent,S.P,S.R,clr_s,clr_w,bSolid,bWire);}
	virtual void __stdcall DrawCylinder(const Fmatrix& parent, const Fvector3& center, const Fvector3& dir, float height, float radius, unsigned int clr_s, unsigned int clr_w, BOOL bSolid, BOOL bWire);
	virtual void __stdcall DrawCone	(const Fmatrix& parent, const Fvector3& apex, const Fvector3& dir, float height, float radius, unsigned int clr_s, unsigned int clr_w, BOOL bSolid, BOOL bWire);
	virtual void __stdcall DrawPlane	(const Fvector3& center, const Fvector2& scale, const Fvector3& rotate, unsigned int clr_s, unsigned int clr_w, BOOL bCull, BOOL bSolid, BOOL bWire);
	virtual void __stdcall DrawPlane	(const Fvector3& p, const Fvector3& n, const Fvector2& scale, unsigned int clr_s, unsigned int clr_w, BOOL bCull, BOOL bSolid, BOOL bWire);
	virtual void __stdcall DrawRectangle(const Fvector3& o, const Fvector3& u, const Fvector3& v, unsigned int clr_s, unsigned int clr_w, BOOL bSolid, BOOL bWire);

	virtual void __stdcall DrawGrid	();
	virtual void __stdcall DrawPivot	(const Fvector3& pos, float sz=5.f);
	virtual void __stdcall DrawAxis	(const Fmatrix& T);
	virtual void __stdcall DrawObjectAxis(const Fmatrix& T, float sz, BOOL sel);
	virtual void __stdcall DrawSelectionRect(const Ivector2& m_SelStart, const Ivector2& m_SelEnd);

	virtual void __stdcall DrawPrimitiveL(D3DPRIMITIVETYPE pt, unsigned int pc, Fvector3* vertices, int vc, unsigned int color, BOOL bCull, BOOL bCycle);
	virtual void __stdcall DrawPrimitiveTL(D3DPRIMITIVETYPE pt, unsigned int pc, FVF::TL* vertices, int vc, BOOL bCull, BOOL bCycle);
	virtual void __stdcall DrawPrimitiveLIT(D3DPRIMITIVETYPE pt, unsigned int pc, FVF::LIT* vertices, int vc, BOOL bCull, BOOL bCycle);

	virtual void __stdcall OutText	(const Fvector3& pos, const char* text, unsigned int color=0xFF000000, unsigned int shadow_color=0xFF909090);

	virtual void 			OnRender		();
};
extern CDrawUtilities DU;
