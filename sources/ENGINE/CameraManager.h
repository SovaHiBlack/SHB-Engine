#pragma once

#include "Camera_defs.h"

struct ENGINE_API SPPInfo {
	struct SColor{
		float r, g, b;
		SColor					(){}
		SColor					(float _r, float _g, float _b):r(_r),g(_g),b(_b){}
		inline operator u32()										{
			int		_r	= clampr	(iFloor(r*255.f+.5f),0,255);
			int		_g	= clampr	(iFloor(g*255.f+.5f),0,255);
			int		_b	= clampr	(iFloor(b*255.f+.5f),0,255);
			return color_rgba		(_r,_g,_b,0);
		}
		inline SColor& operator +=	(const SColor &ppi)				{
			r += ppi.r; g += ppi.g; b += ppi.b; 
			return *this;
		}
		inline SColor& operator -=	(const SColor &ppi)				{
			r -= ppi.r; g -= ppi.g; b -= ppi.b; 
			return *this;
		}
		inline SColor& set			(float _r, float _g, float _b)	{
			r=_r;g=_g;b=_b;
			return *this;
		}
	};
	float		blur, gray;
	struct SDuality { 
		float h, v; 
		SDuality				(){}
		SDuality				(float _h, float _v):h(_h),v(_v){}
		inline SDuality& set		(float _h, float _v)			{
			h=_h;v=_v;
			return *this;
		}
	} duality;
	struct SNoise	{
		float		intensity, grain;
		float		fps;
		SNoise					(){}
		SNoise					(float _i, float _g, float _f):intensity(_i),grain(_g),fps(_f){}
		inline SNoise& set			(float _i, float _g, float _f){
			intensity=_i;grain=_g;fps=_f;
			return *this;
		}
	} noise;

	SColor		color_base;
	SColor		color_gray;
	SColor		color_add;

	inline SPPInfo& operator += (const SPPInfo &ppi) {
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
	inline SPPInfo& operator -= (const SPPInfo &ppi) {
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
	SPPInfo&	lerp(const SPPInfo& def, const SPPInfo& to, float factor);
	void		validate(const char* str);
};

//DEFINE_VECTOR				(CEffectorCam*,EffectorCamVec,EffectorCamIt);
using EffectorCamVec = xr_vector<CEffectorCam*>;
using EffectorCamIt = EffectorCamVec::iterator;
//DEFINE_VECTOR				(CEffectorPP*,EffectorPPVec,EffectorPPIt);
using EffectorPPVec = xr_vector<CEffectorPP*>;
using EffectorPPIt = EffectorPPVec::iterator;

class ENGINE_API CCameraManager
{
	Fvector3					vPosition;
	Fvector3					vDirection;
	Fvector3					vNormal;
	Fvector3					vRight;

	EffectorCamVec			m_EffectorsCam;
	EffectorCamVec			m_EffectorsCam_added_deffered;
	EffectorCamVec			m_EffectorsCam_removed_deffered;
	EffectorPPVec			m_EffectorsPP;

	float					fFov;
	float					fFar;
	float					fAspect;
	bool					m_bAutoApply;
	SPPInfo					pp_affected;
	void					UpdateDeffered();

public:

#ifdef DEBUG	
	u32						dbg_upd_frame;
#endif // DEBUG

	void					Dump					(void);
	CEffectorCam*			AddCamEffector			(CEffectorCam*			ef);
	CEffectorCam*			GetCamEffector			(ECameraEffectorType		type);
	void					RemoveCamEffector		(ECameraEffectorType		type);

	CEffectorPP*			GetPPEffector			(EEffectorPostProcessType	type);
	CEffectorPP*			AddPPEffector			(CEffectorPP*		ef);
	void					RemovePPEffector		(EEffectorPostProcessType	type);

	inline Fvector3				Pos					()	const { return vPosition;	}
	inline Fvector3				Dir					()	const { return vDirection;}
	inline Fvector3				Up					()	const { return vNormal;	}
	inline Fvector3				Right				()	const { return vRight;	}
	
	inline void					camera_Matrix		(Fmatrix& M){M.set(vRight,vNormal,vDirection,vPosition);}
	void					Update				(const Fvector3& P, const Fvector3& D, const Fvector3& N, float fFOV_Dest, float fASPECT_Dest, float fFAR_Dest, u32 flags=0);
	void					Update				(const CCameraBase* C);
	void					ApplyDevice			(float _viewport_near);
	static void				ResetPP				();

							CCameraManager		(bool bApplyOnUpdate);
							~CCameraManager		();
};

ENGINE_API extern SPPInfo					pp_identity;
ENGINE_API extern SPPInfo					pp_zero;

ENGINE_API extern float psCamInert;
ENGINE_API extern float psCamSlideInert;
