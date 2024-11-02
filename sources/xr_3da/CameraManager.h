#pragma once

#include "CameraDefs.h"

struct ENGINE_API SPPInfo {
	struct SColor{
		f32 r;
		f32 g;
		f32 b;
		SColor					(){}
		SColor					(f32 _r, f32 _g, f32 _b):r(_r),g(_g),b(_b){}
		IC operator u32()										{
			int		_r	= clampr	(iFloor(r*255.f+.5f),0,255);
			int		_g	= clampr	(iFloor(g*255.f+.5f),0,255);
			int		_b	= clampr	(iFloor(b*255.f+.5f),0,255);
			return color_rgba		(_r,_g,_b,0);
		}
		IC SColor& operator +=	(const SColor &ppi)				{
			r += ppi.r; g += ppi.g; b += ppi.b; 
			return *this;
		}
		IC SColor& operator -=	(const SColor &ppi)				{
			r -= ppi.r; g -= ppi.g; b -= ppi.b; 
			return *this;
		}
		IC SColor& set			(f32 _r, f32 _g, f32 _b)	{
			r=_r;g=_g;b=_b;
			return *this;
		}
	};
	f32		blur;
	f32		gray;
	struct SDuality { 
		f32 h;
		f32 v;
		SDuality				(){}
		SDuality				(f32 _h, f32 _v):h(_h),v(_v){}
		IC SDuality& set		(f32 _h, f32 _v)			{
			h=_h;v=_v;
			return *this;
		}
	} duality;
	struct SNoise	{
		f32		intensity;
		f32		grain;
		f32		fps;
		SNoise					(){}
		SNoise					(f32 _i, f32 _g, f32 _f):intensity(_i),grain(_g),fps(_f){}
		IC SNoise& set			(f32 _i, f32 _g, f32 _f){
			intensity=_i;grain=_g;fps=_f;
			return *this;
		}
	} noise;

	SColor		color_base;
	SColor		color_gray;
	SColor		color_add;

	IC SPPInfo& operator += (const SPPInfo &ppi) {
		blur		+= ppi.blur;
		gray		+= ppi.gray;
		duality.h	+= ppi.duality.h; duality.v += ppi.duality.v;
		noise.intensity += ppi.noise.intensity; noise.grain += ppi.noise.grain;
		noise.fps	+= ppi.noise.fps;
		color_base	+= ppi.color_base;
		color_gray	+= ppi.color_gray;
		color_add	+= ppi.color_add;
		return *this;
	}
	IC SPPInfo& operator -= (const SPPInfo &ppi) {
		blur		-= ppi.blur;
		gray		-= ppi.gray;
		duality.h	-= ppi.duality.h; duality.v -= ppi.duality.v;
		noise.intensity -= ppi.noise.intensity; noise.grain -= ppi.noise.grain;
		noise.fps	-= ppi.noise.fps;
		color_base	-= ppi.color_base;
		color_gray	-= ppi.color_gray;
		color_add	-= ppi.color_add;
		return *this;
	}
	void normalize		();
	SPPInfo				()
	{
		blur = gray = duality.h = duality.v = 0;
		noise.intensity=0; noise.grain = 1; noise.fps = 10;
		color_base.set	(.5f,	.5f,	.5f);
		color_gray.set	(.333f, .333f,	.333f);
		color_add.set	(0.f,	0.f,	0.f);
	}
	SPPInfo&	lerp(const SPPInfo& def, const SPPInfo& to, f32 factor);
	void		validate(pcstr str);
};

DEFINE_VECTOR				(CEffectorCam*,EffectorCamVec,EffectorCamIt);
DEFINE_VECTOR				(CEffectorPP*,EffectorPPVec,EffectorPPIt);

class ENGINE_API CCameraManager
{
	fVector3					vPosition;
	fVector3					vDirection;
	fVector3					vNormal;
	fVector3					vRight;

	EffectorCamVec			m_EffectorsCam;
	EffectorCamVec			m_EffectorsCam_added_deffered;
	EffectorCamVec			m_EffectorsCam_removed_deffered;
	EffectorPPVec			m_EffectorsPP;

	f32						fFov;
	f32						fFar;
	f32						fAspect;
	bool					m_bAutoApply;
	SPPInfo					pp_affected;
	void					UpdateDeffered();

public:

#ifdef DEBUG	
	u32						dbg_upd_frame;
#endif // DEBUG

	void					Dump					(void);
	CEffectorCam*			AddCamEffector			(CEffectorCam*			ef);
	CEffectorCam*			GetCamEffector			(ECamEffectorType		type);
	void					RemoveCamEffector		(ECamEffectorType		type);

	CEffectorPP*			GetPPEffector			(EEffectorPPType	type);
	CEffectorPP*			AddPPEffector			(CEffectorPP*		ef);
	void					RemovePPEffector		(EEffectorPPType	type);

	IC fVector3				Pos					()	const { return vPosition;	}
	IC fVector3				Dir					()	const { return vDirection;}
	IC fVector3				Up					()	const { return vNormal;	}
	IC fVector3				Right				()	const { return vRight;	}
	
	IC void					camera_Matrix		(fMatrix4x4& M){M.set(vRight,vNormal,vDirection,vPosition);}
	void					Update				(const fVector3& P, const fVector3& D, const fVector3& N, f32 fFOV_Dest, f32 fASPECT_Dest, f32 fFAR_Dest, u32 flags=0);
	void					Update				(const CCameraBase* C);
	void					ApplyDevice			(f32 _viewport_near);
	static void				ResetPP				();

							CCameraManager		(bool bApplyOnUpdate);
							~CCameraManager		();
};
ENGINE_API extern SPPInfo					pp_identity;
ENGINE_API extern SPPInfo					pp_zero;

ENGINE_API extern f32 psCamInert;
ENGINE_API extern f32 psCamSlideInert;
