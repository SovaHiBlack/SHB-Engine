#pragma once

#include "xr_collide_defs.h"

class ENGINE_API CIniFile;

class ENGINE_API CLensFlareDescriptor
{
public:
	struct SFlare
	{
		float			fOpacity;
		float			fRadius;
		float			fPosition;
		shared_str			texture;
		shared_str			shader;
		ref_shader		hShader;
		SFlare()		{ fOpacity = fRadius = fPosition = 0; }
	};
	struct SSource: public SFlare
	{
		BOOL			ignore_color;
	};
//	DEFINE_VECTOR		(SFlare,FlareVec,FlareIt);
	using FlareVec = xr_vector<SFlare>;
	using FlareIt = FlareVec::iterator;
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

	float				m_StateBlendUpSpeed;
	float				m_StateBlendDnSpeed;
	
	void				SetGradient		(float fMaxRadius, float fOpacity, const char* tex_name, const char* sh_name);
	void				SetSource		(float fRadius, BOOL ign_color, const char* tex_name, const char* sh_name);
	void				AddFlare		(float fRadius, float fOpacity, float fPosition, const char* tex_name, const char* sh_name);
	ref_shader			CreateShader	(const char* tex_name, const char* sh_name);

	shared_str			section;
public:
						CLensFlareDescriptor(){m_Flags.zero();section=0;m_StateBlendUpSpeed=m_StateBlendDnSpeed=0.1f;}
	void				load				(CIniFile* pIni, const char* section);
	void 				OnDeviceCreate	();
	void 				OnDeviceDestroy	();
};

//DEFINE_VECTOR(CLensFlareDescriptor,LensFlareDescVec,LensFlareDescIt);
using LensFlareDescVec = xr_vector<CLensFlareDescriptor>;
using LensFlareDescIt = LensFlareDescVec::iterator;

class ENGINE_API CLensFlare
{
private:
	collide::rq_results	r_dest;

protected:
	float				fBlend;
	u32					dwFrame;

	Fvector3				vSunDir;
	Fvector3				vecLight;
	Fvector3				vecX;
	Fvector3 vecY;
	Fvector3 vecDir;
	Fvector3 vecAxis;
	Fvector3 vecCenter;
	BOOL				bRender;

	// variable
	Fcolor				LightColor;
	float				fGradientValue;

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
	float				m_StateBlend;

public:
	collide::ray_cache	m_ray_cache;

public:
						CLensFlare		();
	virtual				~CLensFlare		();

	void				OnFrame			(int id);
	void __fastcall		Render			(BOOL bSun, BOOL bFlares, BOOL bGradient);
	void 				OnDeviceCreate	();         
	void 				OnDeviceDestroy	();

	int					AppendDef		(CIniFile* pIni, const char* sect);

	void				Invalidate		(){m_State=lfsNone;}
};
