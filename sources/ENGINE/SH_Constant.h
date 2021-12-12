#pragma once

#include "WaveForm.h"

class	ENGINE_API	IReader;
class	ENGINE_API	IWriter;

class	ENGINE_API	CConstant	: public xr_resource_named									{
public:
	enum			{ modeProgrammable=0, modeWaveForm	};

	Fcolor			const_float;
	unsigned int				const_dword;

	unsigned int				dwFrame;
	unsigned int				dwMode;
	WaveForm		_R;
	WaveForm		_G;
	WaveForm		_B;
	WaveForm		_A;

	CConstant		()
	{
		Memory.mem_fill	(this,0,sizeof(CConstant));
	}
	inline void			set_float	(float r, float g, float b, float a)
	{
		const_float.set	(r,g,b,a);
		const_dword		= const_float.get();
	}
	inline void			set_float	(Fcolor& c)
	{
		const_float.set	(c);
		const_dword		= const_float.get();
	}
	inline void			set_dword	(unsigned int c)
	{
		const_float.set(c);
		const_dword		= c;
	}
	void			Calculate	();
	inline	BOOL		Similar		(CConstant& C)		// comare by modes and params
	{
		if (dwMode!=C.dwMode)	return FALSE;
		if (!_R.Similar(C._R))	return FALSE;
		if (!_G.Similar(C._G))	return FALSE;
		if (!_B.Similar(C._B))	return FALSE;
		if (!_A.Similar(C._A))	return FALSE;
		return TRUE;
	}
	void			Load		(IReader* fs);
	void			Save		(IWriter* fs);
};

typedef	resptr_core<CConstant,resptr_base<CConstant> >	
	ref_constant_obsolette;
