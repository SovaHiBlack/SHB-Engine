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
    virtual void __stdcall DrawCross			(const Fvector3& p, float szx1, float szy1, float szz1, float szx2, float szy2, float szz2, u32 clr, BOOL bRot45=false)=0;
    virtual void __stdcall DrawCross			(const Fvector3& p, float sz, u32 clr, BOOL bRot45=false)=0;
    virtual void __stdcall DrawFlag				(const Fvector3& p, float heading, float height, float sz, float sz_fl, u32 clr, BOOL bDrawEntity)=0;
    virtual void __stdcall DrawRomboid			(const Fvector3& p, float radius, u32 clr)=0;
    virtual void __stdcall DrawJoint			(const Fvector3& p, float radius, u32 clr)=0;

    virtual void __stdcall DrawSpotLight	   	(const Fvector3& p, const Fvector3& d, float range, float phi, u32 clr)=0;
    virtual void __stdcall DrawDirectionalLight	(const Fvector3& p, const Fvector3& d, float radius, float range, u32 clr)=0;
    virtual void __stdcall DrawPointLight	   	(const Fvector3& p, float radius, u32 clr)=0;

    virtual void __stdcall DrawSound		   	(const Fvector3& p, float radius, u32 clr)=0;
    virtual void __stdcall DrawLineSphere	   	(const Fvector3& p, float radius, u32 clr, BOOL bCross)=0;

	virtual void __stdcall dbgDrawPlacement		(const Fvector3& p, int sz, u32 clr, const char* caption=0, u32 clr_font=0xffffffff)=0;
    virtual void __stdcall dbgDrawVert			(const Fvector3& p0, u32 clr, const char* caption=0)=0;
    virtual void __stdcall dbgDrawEdge			(const Fvector3& p0, const Fvector3& p1, u32 clr, const char* caption=0)=0;
    virtual void __stdcall dbgDrawFace			(const Fvector3& p0, const Fvector3& p1, const Fvector3& p2, u32 clr, const char* caption=0)=0;

    virtual void __stdcall DrawFace				(const Fvector3& p0, const Fvector3& p1, const Fvector3& p2, u32 clr_s, u32 clr_w, BOOL bSolid, BOOL bWire)=0;
    virtual void __stdcall DrawLine				(const Fvector3& p0, const Fvector3& p1, u32 clr)=0;
    virtual void __stdcall DrawLine				(const Fvector3* p, u32 clr)=0;
    virtual void __stdcall DrawLink				(const Fvector3& p0, const Fvector3& p1, float sz, u32 clr)=0;
    virtual void __stdcall DrawFaceNormal	   	(const Fvector3& p0, const Fvector3& p1, const Fvector3& p2, float size, u32 clr)=0;
    virtual void __stdcall DrawFaceNormal	   	(const Fvector3* p, float size, u32 clr)=0;
    virtual void __stdcall DrawFaceNormal	   	(const Fvector3& C, const Fvector3& N, float size, u32 clr)=0;
    virtual void __stdcall DrawSelectionBox		(const Fvector3& center, const Fvector3& size, u32* c=0)=0;
    virtual void __stdcall DrawSelectionBox		(const Fbox& box, u32* c=0)=0;
    virtual void __stdcall DrawIdentSphere		(BOOL bSolid, BOOL bWire, u32 clr_s, u32 clr_w)=0;
    virtual void __stdcall DrawIdentSpherePart	(BOOL bSolid, BOOL bWire, u32 clr_s, u32 clr_w)=0;
    virtual void __stdcall DrawIdentCone	   	(BOOL bSolid, BOOL bWire, u32 clr_s, u32 clr_w)=0;
	virtual void __stdcall DrawIdentCylinder   	(BOOL bSolid, BOOL bWire, u32 clr_s, u32 clr_w)=0;
    virtual void __stdcall DrawIdentBox			(BOOL bSolid, BOOL bWire, u32 clr_s, u32 clr_w)=0;

    virtual void __stdcall DrawBox				(const Fvector3& offs, const Fvector3& Size, BOOL bSolid, BOOL bWire, u32 clr_s, u32 clr_w)=0;
    virtual void __stdcall DrawAABB				(const Fvector3& p0, const Fvector3& p1, u32 clr_s, u32 clr_w, BOOL bSolid, BOOL bWire)=0;
    virtual void __stdcall DrawAABB				(const Fmatrix& parent, const Fvector3& center, const Fvector3& size, u32 clr_s, u32 clr_w, BOOL bSolid, BOOL bWire)=0;
    virtual void __stdcall DrawOBB				(const Fmatrix& parent, const Fobb& box, u32 clr_s, u32 clr_w)=0;
	virtual void __stdcall DrawSphere		   	(const Fmatrix& parent, const Fvector3& center, float radius, u32 clr_s, u32 clr_w, BOOL bSolid, BOOL bWire)=0;
	virtual void __stdcall DrawSphere		   	(const Fmatrix& parent, const Fsphere& S, u32 clr_s, u32 clr_w, BOOL bSolid, BOOL bWire)=0;
	virtual void __stdcall DrawCylinder			(const Fmatrix& parent, const Fvector3& center, const Fvector3& dir, float height, float radius, u32 clr_s, u32 clr_w, BOOL bSolid, BOOL bWire)=0;
	virtual void __stdcall DrawCone				(const Fmatrix& parent, const Fvector3& apex, const Fvector3& dir, float height, float radius, u32 clr_s, u32 clr_w, BOOL bSolid, BOOL bWire)=0;
    virtual void __stdcall DrawPlane		   	(const Fvector3& center, const Fvector2& scale, const Fvector3& rotate, u32 clr_s, u32 clr_w, BOOL bCull, BOOL bSolid, BOOL bWire)=0;
    virtual void __stdcall DrawPlane		   	(const Fvector3& p, const Fvector3& n, const Fvector2& scale, u32 clr_s, u32 clr_w, BOOL bCull, BOOL bSolid, BOOL bWire)=0;
    virtual void __stdcall DrawRectangle	   	(const Fvector3& o, const Fvector3& u, const Fvector3& v, u32 clr_s, u32 clr_w, BOOL bSolid, BOOL bWire)=0;

    virtual void __stdcall DrawGrid				()=0;
    virtual void __stdcall DrawPivot		   	(const Fvector3& pos, float sz=5.f)=0;
	virtual void __stdcall DrawAxis				(const Fmatrix& T)=0;
	virtual void __stdcall DrawObjectAxis	   	(const Fmatrix& T, float sz, BOOL sel)=0;
	virtual void __stdcall DrawSelectionRect   	(const Ivector2& m_SelStart, const Ivector2& m_SelEnd)=0;

    virtual void __stdcall OutText				(const Fvector3& pos, const char* text, u32 color=0xFF000000, u32 shadow_color=0xFF909090)=0;
};
//----------------------------------------------------
#endif

