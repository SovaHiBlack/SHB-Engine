#pragma once

#include "Camera_defs.h"

struct ENGINE_API SPostProcessInfo
{
	struct SColor
	{
		float									r;
		float									g;
		float									b;
							SColor				( )
		{ }
							SColor				(float _r, float _g, float _b) : r(_r), g(_g), b(_b)
		{ }
		inline				operator unsigned int( )
		{
			int _r = clampr(iFloor(r * 255.0f + 0.5f), 0, 255);
			int _g = clampr(iFloor(g * 255.0f + 0.5f), 0, 255);
			int _b = clampr(iFloor(b * 255.0f + 0.5f), 0, 255);
			return color_rgba(_r, _g, _b, 0);
		}
		inline SColor&		operator +=			(const SColor& ppi)
		{
			r += ppi.r;
			g += ppi.g;
			b += ppi.b;
			return *this;
		}
		inline SColor&		operator -=			(const SColor& ppi)
		{
			r -= ppi.r;
			g -= ppi.g;
			b -= ppi.b;
			return *this;
		}
		inline SColor&		set					(float _r, float _g, float _b)
		{
			r = _r;
			g = _g;
			b = _b;
			return *this;
		}
	};
	float										blur;
	float										gray;
	struct SDuality
	{
		float									h;
		float									v;
							SDuality			( )
		{ }
							SDuality			(float _h, float _v) : h(_h), v(_v)
		{ }
		inline SDuality&	set					(float _h, float _v)
		{
			h = _h;
			v = _v;
			return *this;
		}
	}											duality;
	struct SNoise
	{
		float									intensity;
		float									grain;
		float									fps;
							SNoise				( )
		{ }
							SNoise				(float _i, float _g, float _f) : intensity(_i), grain(_g), fps(_f)
		{ }
		inline SNoise&		set					(float _i, float _g, float _f)
		{
			intensity = _i;
			grain = _g;
			fps = _f;
			return *this;
		}
	}											noise;

	SColor										color_base;
	SColor										color_gray;
	SColor										color_add;

	inline SPostProcessInfo&			operator +=			(const SPostProcessInfo& ppi)
	{
		blur += ppi.blur;
		gray += ppi.gray;
		duality.h += ppi.duality.h; duality.v += ppi.duality.v;
		noise.intensity += ppi.noise.intensity; noise.grain += ppi.noise.grain;
		noise.fps += ppi.noise.fps;
		color_base += ppi.color_base;
		color_gray += ppi.color_gray;
		color_add += ppi.color_add;
		return *this;
	}
	inline SPostProcessInfo&			operator -=			(const SPostProcessInfo& ppi)
	{
		blur -= ppi.blur;
		gray -= ppi.gray;
		duality.h -= ppi.duality.h; duality.v -= ppi.duality.v;
		noise.intensity -= ppi.noise.intensity; noise.grain -= ppi.noise.grain;
		noise.fps -= ppi.noise.fps;
		color_base -= ppi.color_base;
		color_gray -= ppi.color_gray;
		color_add -= ppi.color_add;
		return *this;
	}
	void					normalize			( );
	SPostProcessInfo( )
	{
		blur = gray = duality.h = duality.v = 0.0f;
		noise.intensity = 0.0f;
		noise.grain = 1.0f;
		noise.fps = 10.0f;
		color_base.set(0.5f, 0.5f, 0.5f);
		color_gray.set(0.333f, 0.333f, 0.333f);
		color_add.set(0.0f, 0.0f, 0.0f);
	}
	SPostProcessInfo&		lerp				(const SPostProcessInfo& def, const SPostProcessInfo& to, float factor);
	void					validate			(const char* str);
};

using EffectorCamVec							= xr_vector<CEffectorCam*>;
using EffectorCamVec_it							= EffectorCamVec::iterator;

using EffectorPostProcessVec					= xr_vector<CEffectorPostProcess*>;
using EffectorPostProcessVec_it					= EffectorPostProcessVec::iterator;

class ENGINE_API CCameraManager
{
	Fvector3									vPosition;
	Fvector3									vDirection;
	Fvector3									vNormal;
	Fvector3									vRight;

	EffectorCamVec								m_EffectorsCam;
	EffectorCamVec								m_EffectorsCam_added_deffered;
	EffectorCamVec								m_EffectorsCam_removed_deffered;
	EffectorPostProcessVec						m_EffectorsPP;

	float										fFov;
	float										fFar;
	float										fAspect;
	bool										m_bAutoApply;
	SPostProcessInfo							pp_affected;
	void					UpdateDeffered		( );

public:

#ifdef DEBUG
	unsigned int											dbg_upd_frame;
#endif // def DEBUG

	void					Dump				( );

	CEffectorCam*			addCamEffector		(CEffectorCam* ef);
	CEffectorCam*			GetCamEffector		(ECameraEffectorType type);
	void					RemoveCamEffector	(ECameraEffectorType type);

	CEffectorPostProcess*	GetPPEffector		(EEffectorPostProcessType type);
	CEffectorPostProcess*	AddPPEffector		(CEffectorPostProcess* ef);
	void					RemovePPEffector	(EEffectorPostProcessType type);

	inline Fvector3			Pos					( ) const
	{
		return vPosition;
	}
	inline Fvector3			Dir					( ) const
	{
		return vDirection;
	}
	inline Fvector3			Up					( ) const
	{
		return vNormal;
	}
	inline Fvector3			Right				( ) const
	{
		return vRight;
	}

	inline void				camera_Matrix		(Fmatrix& M)
	{
		M.set(vRight, vNormal, vDirection, vPosition);
	}
	void					Update				(const Fvector3& P, const Fvector3& D, const Fvector3& N, float fFOV_Dest, float fASPECT_Dest, float fFAR_Dest, unsigned int flags = 0);
	void					Update				(const CCameraBase* C);
	void					ApplyDevice			(float _viewport_near);
	static void				ResetPP				( );

							CCameraManager		(bool bApplyOnUpdate);
							~CCameraManager		( );
};

ENGINE_API extern SPostProcessInfo				pp_identity;
ENGINE_API extern SPostProcessInfo				pp_zero;

ENGINE_API extern float							psCamInert;
ENGINE_API extern float							psCamSlideInert;
