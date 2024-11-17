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
	virtual void __stdcall DrawCross			(const fVector3& p, f32 szx1, f32 szy1, f32 szz1, f32 szx2, f32 szy2, f32 szz2, u32 clr, BOOL bRot45 = false) = 0;
	virtual void __stdcall DrawCross			(const fVector3& p, f32 sz, u32 clr, BOOL bRot45 = false) = 0;
	virtual void __stdcall DrawFlag				(const fVector3& p, f32 heading, f32 height, f32 sz, f32 sz_fl, u32 clr, BOOL bDrawEntity) = 0;
	virtual void __stdcall DrawRomboid			(const fVector3& p, f32 radius, u32 clr) = 0;
	virtual void __stdcall DrawJoint			(const fVector3& p, f32 radius, u32 clr) = 0;

	virtual void __stdcall DrawSpotLight		(const fVector3& p, const fVector3& d, f32 range, f32 phi, u32 clr) = 0;
	virtual void __stdcall DrawDirectionalLight	(const fVector3& p, const fVector3& d, f32 radius, f32 range, u32 clr) = 0;
	virtual void __stdcall DrawPointLight		(const fVector3& p, f32 radius, u32 clr) = 0;

	virtual void __stdcall DrawSound			(const fVector3& p, f32 radius, u32 clr) = 0;
	virtual void __stdcall DrawLineSphere		(const fVector3& p, f32 radius, u32 clr, BOOL bCross) = 0;

	virtual void __stdcall dbgDrawPlacement		(const fVector3& p, s32 sz, u32 clr, pcstr caption = 0, u32 clr_font = 0xffffffff) = 0;
	virtual void __stdcall dbgDrawVert			(const fVector3& p0, u32 clr, pcstr caption = 0) = 0;
	virtual void __stdcall dbgDrawEdge			(const fVector3& p0, const fVector3& p1, u32 clr, pcstr caption = 0) = 0;
	virtual void __stdcall dbgDrawFace			(const fVector3& p0, const fVector3& p1, const fVector3& p2, u32 clr, pcstr caption = 0) = 0;

	virtual void __stdcall DrawFace				(const fVector3& p0, const fVector3& p1, const fVector3& p2, u32 clr_s, u32 clr_w, BOOL bSolid, BOOL bWire) = 0;
	virtual void __stdcall DrawLine				(const fVector3& p0, const fVector3& p1, u32 clr) = 0;
	virtual void __stdcall DrawLine				(const fVector3* p, u32 clr) = 0;
	virtual void __stdcall DrawLink				(const fVector3& p0, const fVector3& p1, f32 sz, u32 clr) = 0;
	virtual void __stdcall DrawFaceNormal		(const fVector3& p0, const fVector3& p1, const fVector3& p2, f32 size, u32 clr) = 0;
	virtual void __stdcall DrawFaceNormal		(const fVector3* p, f32 size, u32 clr) = 0;
	virtual void __stdcall DrawFaceNormal		(const fVector3& C, const fVector3& N, f32 size, u32 clr) = 0;
	virtual void __stdcall DrawSelectionBox		(const fVector3& center, const fVector3& size, u32* c = 0) = 0;
	virtual void __stdcall DrawSelectionBox		(const fBox3& box, u32* c = 0) = 0;
	virtual void __stdcall DrawIdentSphere		(BOOL bSolid, BOOL bWire, u32 clr_s, u32 clr_w) = 0;
	virtual void __stdcall DrawIdentSpherePart	(BOOL bSolid, BOOL bWire, u32 clr_s, u32 clr_w) = 0;
	virtual void __stdcall DrawIdentCone		(BOOL bSolid, BOOL bWire, u32 clr_s, u32 clr_w) = 0;
	virtual void __stdcall DrawIdentCylinder	(BOOL bSolid, BOOL bWire, u32 clr_s, u32 clr_w) = 0;
	virtual void __stdcall DrawIdentBox			(BOOL bSolid, BOOL bWire, u32 clr_s, u32 clr_w) = 0;

	virtual void __stdcall DrawBox				(const fVector3& offs, const fVector3& Size, BOOL bSolid, BOOL bWire, u32 clr_s, u32 clr_w) = 0;
	virtual void __stdcall DrawAABB				(const fVector3& p0, const fVector3& p1, u32 clr_s, u32 clr_w, BOOL bSolid, BOOL bWire) = 0;
	virtual void __stdcall DrawAABB				(const fMatrix4x4& parent, const fVector3& center, const fVector3& size, u32 clr_s, u32 clr_w, BOOL bSolid, BOOL bWire) = 0;
	virtual void __stdcall DrawOBB				(const fMatrix4x4& parent, const fObb& box, u32 clr_s, u32 clr_w) = 0;
	virtual void __stdcall DrawSphere			(const fMatrix4x4& parent, const fVector3& center, f32 radius, u32 clr_s, u32 clr_w, BOOL bSolid, BOOL bWire) = 0;
	virtual void __stdcall DrawSphere			(const fMatrix4x4& parent, const fSphere& S, u32 clr_s, u32 clr_w, BOOL bSolid, BOOL bWire) = 0;
	virtual void __stdcall DrawCylinder			(const fMatrix4x4& parent, const fVector3& center, const fVector3& dir, f32 height, f32 radius, u32 clr_s, u32 clr_w, BOOL bSolid, BOOL bWire) = 0;
	virtual void __stdcall DrawCone				(const fMatrix4x4& parent, const fVector3& apex, const fVector3& dir, f32 height, f32 radius, u32 clr_s, u32 clr_w, BOOL bSolid, BOOL bWire) = 0;
	virtual void __stdcall DrawPlane			(const fVector3& center, const fVector2& scale, const fVector3& rotate, u32 clr_s, u32 clr_w, BOOL bCull, BOOL bSolid, BOOL bWire) = 0;
	virtual void __stdcall DrawPlane			(const fVector3& p, const fVector3& n, const fVector2& scale, u32 clr_s, u32 clr_w, BOOL bCull, BOOL bSolid, BOOL bWire) = 0;
	virtual void __stdcall DrawRectangle		(const fVector3& o, const fVector3& u, const fVector3& v, u32 clr_s, u32 clr_w, BOOL bSolid, BOOL bWire) = 0;

	virtual void __stdcall DrawGrid				( ) = 0;
	virtual void __stdcall DrawPivot			(const fVector3& pos, f32 sz = 5.0f) = 0;
	virtual void __stdcall DrawAxis				(const fMatrix4x4& T) = 0;
	virtual void __stdcall DrawObjectAxis		(const fMatrix4x4& T, f32 sz, BOOL sel) = 0;
	virtual void __stdcall DrawSelectionRect	(const iVector2& m_SelStart, const iVector2& m_SelEnd) = 0;

	virtual void __stdcall OutText				(const fVector3& pos, pcstr text, u32 color = 0xFF000000, u32 shadow_color = 0xFF909090) = 0;
};
//----------------------------------------------------
#endif
