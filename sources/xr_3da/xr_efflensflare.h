#pragma once

#include "xr_collide_defs.h"

class ENGINE_API CInifile;

class ENGINE_API CLensFlareDescriptor
{
public:
	struct SFlare
	{
		f32			fOpacity;
		f32			fRadius;
		f32			fPosition;
        shared_str			texture;
        shared_str			shader;
        ref_shader		hShader;
    	SFlare()		{ fOpacity = fRadius = fPosition = 0; }
	};
    struct SSource: public SFlare
    {
    	BOOL			ignore_color;
    };
    DEFINE_VECTOR		(SFlare,FlareVec,FlareIt);
    FlareVec			m_Flares;

	enum {
    	flFlare 		= (1<<0),
    	flSource		= (1<<1),
    	flGradient 		= (1<<2)
    };
	Flags32				m_Flags;
    
	// source
    SSource				m_Source;
    
	// gradient
    SFlare				m_Gradient;

	f32				m_StateBlendUpSpeed;
	f32				m_StateBlendDnSpeed;
    
	void				SetGradient		(f32 fMaxRadius, f32 fOpacity, pcstr tex_name, pcstr sh_name);
    void				SetSource		(f32 fRadius, BOOL ign_color, pcstr tex_name, pcstr sh_name);
    void				AddFlare		(f32 fRadius, f32 fOpacity, f32 fPosition, pcstr tex_name, pcstr sh_name);
    ref_shader			CreateShader	(pcstr tex_name, pcstr sh_name);

	shared_str			section;
public:
    					CLensFlareDescriptor(){m_Flags.zero();section=0;m_StateBlendUpSpeed=m_StateBlendDnSpeed=0.1f;}
    void				load				(CInifile* pIni, pcstr section);
	void 				OnDeviceCreate	();
	void 				OnDeviceDestroy	();
};
DEFINE_VECTOR(CLensFlareDescriptor,LensFlareDescVec,LensFlareDescIt); 


class ENGINE_API CLensFlare
{
private:
	collide::rq_results	r_dest;

protected:
	F32				fBlend;
	u32					dwFrame;

	Fvector				vSunDir;
	Fvector				vecLight;
	Fvector				vecX, vecY, vecDir, vecAxis, vecCenter;
	BOOL				bRender;

	// variable
    Fcolor				LightColor;
	F32				fGradientValue;

	ref_geom			hGeom;

    LensFlareDescVec	m_Palette;
	CLensFlareDescriptor* m_Current;

    enum LFState{
        lfsNone,
        lfsIdle,
    	lfsHide,
        lfsShow,
    };
    LFState				m_State;
	F32				m_StateBlend;

public:
	collide::ray_cache	m_ray_cache;

public:
						CLensFlare		();
	virtual				~CLensFlare		();

	void				OnFrame			(int id);
    void __fastcall		Render			(BOOL bSun, BOOL bFlares, BOOL bGradient);
	void 				OnDeviceCreate	();         
	void 				OnDeviceDestroy	();

    int					AppendDef		(CInifile* pIni, pcstr sect);

	void				Invalidate		(){m_State=lfsNone;}
};
