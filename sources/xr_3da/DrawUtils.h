//----------------------------------------------------
// file: DrawUtils.h
//----------------------------------------------------

#ifndef DrawUtilsH
#define DrawUtilsH
//----------------------------------------------------
// Utilities
//----------------------------------------------------
class CDUInterface
{
public:
	//----------------------------------------------------
	virtual void __stdcall DrawCross			(const Fvector& p, f32 szx1, f32 szy1, f32 szz1, f32 szx2, f32 szy2, f32 szz2, u32 clr, BOOL bRot45=false)=0;
	virtual void __stdcall DrawCross			(const Fvector& p, f32 sz, u32 clr, BOOL bRot45=false)=0;
	virtual void __stdcall DrawFlag				(const Fvector& p, f32 heading, f32 height, f32 sz, f32 sz_fl, u32 clr, BOOL bDrawEntity)=0;
	virtual void __stdcall DrawRomboid			(const Fvector& p, f32 radius, u32 clr)=0;
	virtual void __stdcall DrawJoint			(const Fvector& p, f32 radius, u32 clr)=0;

	virtual void __stdcall DrawSpotLight	   	(const Fvector& p, const Fvector& d, f32 range, f32 phi, u32 clr)=0;
	virtual void __stdcall DrawDirectionalLight	(const Fvector& p, const Fvector& d, f32 radius, f32 range, u32 clr)=0;
	virtual void __stdcall DrawPointLight	   	(const Fvector& p, f32 radius, u32 clr)=0;

	virtual void __stdcall DrawSound		   	(const Fvector& p, f32 radius, u32 clr)=0;
	virtual void __stdcall DrawLineSphere	   	(const Fvector& p, f32 radius, u32 clr, BOOL bCross)=0;

	virtual void __stdcall dbgDrawPlacement		(const Fvector& p, int sz, u32 clr, pcstr caption=0, u32 clr_font=0xffffffff)=0;
	virtual void __stdcall dbgDrawVert			(const Fvector& p0, u32 clr, pcstr caption=0)=0;
	virtual void __stdcall dbgDrawEdge			(const Fvector& p0,	const Fvector& p1, u32 clr, pcstr caption=0)=0;
	virtual void __stdcall dbgDrawFace			(const Fvector& p0,	const Fvector& p1, const Fvector& p2, u32 clr, pcstr caption=0)=0;

	virtual void __stdcall DrawFace				(const Fvector& p0,	const Fvector& p1, const Fvector& p2, u32 clr_s, u32 clr_w, BOOL bSolid, BOOL bWire)=0;
	virtual void __stdcall DrawLine				(const Fvector& p0,	const Fvector& p1, u32 clr)=0;
	virtual void __stdcall DrawLine				(const Fvector* p, u32 clr)=0;
	virtual void __stdcall DrawLink				(const Fvector& p0, const Fvector& p1, f32 sz, u32 clr)=0;
	virtual void __stdcall DrawFaceNormal	   	(const Fvector& p0, const Fvector& p1, const Fvector& p2, f32 size, u32 clr)=0;
	virtual void __stdcall DrawFaceNormal	   	(const Fvector* p, f32 size, u32 clr)=0;
	virtual void __stdcall DrawFaceNormal	   	(const Fvector& C, const Fvector& N, f32 size, u32 clr)=0;
	virtual void __stdcall DrawSelectionBox		(const Fvector& center, const Fvector& size, u32* c=0)=0;
	virtual void __stdcall DrawSelectionBox		(const fBox3& box, u32* c=0)=0;
	virtual void __stdcall DrawIdentSphere		(BOOL bSolid, BOOL bWire, u32 clr_s, u32 clr_w)=0;
	virtual void __stdcall DrawIdentSpherePart	(BOOL bSolid, BOOL bWire, u32 clr_s, u32 clr_w)=0;
	virtual void __stdcall DrawIdentCone	   	(BOOL bSolid, BOOL bWire, u32 clr_s, u32 clr_w)=0;
	virtual void __stdcall DrawIdentCylinder   	(BOOL bSolid, BOOL bWire, u32 clr_s, u32 clr_w)=0;
	virtual void __stdcall DrawIdentBox			(BOOL bSolid, BOOL bWire, u32 clr_s, u32 clr_w)=0;

	virtual void __stdcall DrawBox				(const Fvector& offs, const Fvector& Size, BOOL bSolid, BOOL bWire, u32 clr_s, u32 clr_w)=0;
	virtual void __stdcall DrawAABB				(const Fvector& p0, const Fvector& p1, u32 clr_s, u32 clr_w, BOOL bSolid, BOOL bWire)=0;
	virtual void __stdcall DrawAABB				(const fMatrix4x4& parent, const Fvector& center, const Fvector& size, u32 clr_s, u32 clr_w, BOOL bSolid, BOOL bWire)=0;
	virtual void __stdcall DrawOBB				(const fMatrix4x4& parent, const fObb& box, u32 clr_s, u32 clr_w)=0;
	virtual void __stdcall DrawSphere		   	(const fMatrix4x4& parent, const Fvector& center, f32 radius, u32 clr_s, u32 clr_w, BOOL bSolid, BOOL bWire)=0;
	virtual void __stdcall DrawSphere		   	(const fMatrix4x4& parent, const fSphere& S, u32 clr_s, u32 clr_w, BOOL bSolid, BOOL bWire)=0;
	virtual void __stdcall DrawCylinder			(const fMatrix4x4& parent, const Fvector& center, const Fvector& dir, f32 height, f32 radius, u32 clr_s, u32 clr_w, BOOL bSolid, BOOL bWire)=0;
	virtual void __stdcall DrawCone				(const fMatrix4x4& parent, const Fvector& apex, const Fvector& dir, f32 height, f32 radius, u32 clr_s, u32 clr_w, BOOL bSolid, BOOL bWire)=0;
	virtual void __stdcall DrawPlane		   	(const Fvector& center, const fVector2& scale, const Fvector& rotate, u32 clr_s, u32 clr_w, BOOL bCull, BOOL bSolid, BOOL bWire)=0;
	virtual void __stdcall DrawPlane		   	(const Fvector& p, const Fvector& n, const fVector2& scale, u32 clr_s, u32 clr_w, BOOL bCull, BOOL bSolid, BOOL bWire)=0;
	virtual void __stdcall DrawRectangle	   	(const Fvector& o, const Fvector& u, const Fvector& v, u32 clr_s, u32 clr_w, BOOL bSolid, BOOL bWire)=0;

	virtual void __stdcall DrawGrid				()=0;
	virtual void __stdcall DrawPivot		   	(const Fvector& pos, f32 sz=5.0f)=0;
	virtual void __stdcall DrawAxis				(const fMatrix4x4& T)=0;
	virtual void __stdcall DrawObjectAxis	   	(const fMatrix4x4& T, f32 sz, BOOL sel)=0;
	virtual void __stdcall DrawSelectionRect   	(const iVector2& m_SelStart, const iVector2& m_SelEnd)=0;

	virtual void __stdcall OutText				(const Fvector& pos, pcstr text, u32 color=0xFF000000, u32 shadow_color=0xFF909090)=0;
};
//----------------------------------------------------
#endif

