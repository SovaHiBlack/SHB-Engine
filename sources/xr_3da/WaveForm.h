#ifndef WAVEFORM_H
#define WAVEFORM_H
#pragma once

#pragma pack(push,4)
struct WaveForm
{
	enum EFunction
	{
		fCONSTANT		= 0,
		fSIN,
		fTRIANGLE,
		fSQUARE,
		fSAWTOOTH,
		fINVSAWTOOTH,
		fFORCE32		= u32(-1)
	};
	IC f32 signf		(f32 t) { return t/_abs(t); }
	IC f32 Func		(f32 t)
	{
		switch (F)
		{
			case fCONSTANT:		return 0;
			case fSIN:			return _sin(t*PI_MUL_2);
			case fTRIANGLE:		return asinf(_sin((t-0.25f)*PI_MUL_2))/PI_DIV_2;
			case fSQUARE:		return signf(_cos(t*PI));
			case fSAWTOOTH:		return atanf(tanf((t+0.5f)*PI))/PI_DIV_2;
			case fINVSAWTOOTH:	return -(atanf(tanf((t+0.5f)*PI))/PI_DIV_2);
		}
		return 0.f;
	}
public:
	EFunction	F;
	f32		arg[4];

	IC f32 Calculate	(f32 t)
	{
		// y = arg0 + arg1*func( (time+arg2)*arg3 )
		f32 x = (t+arg[2])*arg[3];
		return arg[0] + arg[1]*Func(x-floorf(x));
	}

	WaveForm()	{
		F		= fCONSTANT;
		arg[0]	= 0;
		arg[1]	= 1;
		arg[2]	= 0;
		arg[3]	= 1;
	}

	IC	BOOL	Similar	(const WaveForm& W) const
	{
		if (!fsimilar(arg[0],W.arg[0], EPSILON_3))	return FALSE;
		if (!fsimilar(arg[1],W.arg[1], EPSILON_3))	return FALSE;
		if (fis_zero(arg[1], EPSILON_3))				return TRUE;
		if (F	!=	W.F)						return FALSE;
		if (!fsimilar(arg[2],W.arg[2], EPSILON_3))	return FALSE;
		if (!fsimilar(arg[3],W.arg[3], EPSILON_3))	return FALSE;
		return TRUE;
	}
};

#pragma pack(pop)
#endif
