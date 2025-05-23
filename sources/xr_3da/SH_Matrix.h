#pragma once

#include "WaveForm.h"

class	ENGINE_API	IReader;
class	ENGINE_API	IWriter;

class	ENGINE_API	CMatrix	: public xr_resource_named								{
public:
	enum { modeProgrammable=0, modeTCM, modeS_refl, modeC_refl, modeDetail	};
	enum
	{
		tcmScale		= (1<<0),
		tcmRotate		= (1<<1),
		tcmScroll		= (1<<2),
		tcmFORCE32		= u32(-1)
	};

public:
	fMatrix4x4			xform;

	u32				dwFrame;
	u32				dwMode;
    union{
		u32		tcm;				// mask for tc-modifiers
		flags32	tcm_flags;
    };
	WaveForm		scaleU, scaleV;
	WaveForm		rotate;
	WaveForm		scrollU,scrollV;

	CMatrix			()
	{
		Memory.mem_fill	(this,0,sizeof(CMatrix));
	}

	IC void			tc_trans	(fMatrix4x4& T, f32 u, f32 v)
	{
		T.identity	();
		T.m[2][0] = u;
		T.m[2][1] = v;
	}
	void			Calculate	();

	IC	BOOL		Similar		(CMatrix& M)		// comare by modes and params
	{
		if (dwMode!=M.dwMode)				return FALSE;
		if (tcm!=M.tcm)						return FALSE;
		if (!scaleU.Similar(M.scaleU))		return FALSE;
		if (!scaleV.Similar(M.scaleV))		return FALSE;
		if (!rotate.Similar(M.rotate))		return FALSE;
		if (!scrollU.Similar(M.scrollU))	return FALSE;
		if (!scrollV.Similar(M.scrollV))	return FALSE;
		return TRUE;
	}

	void			Load		(IReader* fs);
	void			Save		(IWriter* fs);
};

typedef	resptr_core<CMatrix,resptr_base<CMatrix> >	
	ref_matrix;
