#pragma once

class CRandom
{
private:
	volatile	s32		holdrand;
public:
	CRandom() : holdrand(1)
	{};
	CRandom(s32 _seed) : holdrand(_seed)
	{};

	IC 	void	seed(s32 val)
	{
		holdrand = val;
	}
	IC 	s32		maxI()
	{
		return 32767;
	}

	ICN	s32		randI()
	{
		return(((holdrand = holdrand * 214013L + 2531011L) >> 16) & 0x7fff);
	}
	IC 	s32		randI(s32 max)
	{
		VERIFY(max);  return randI() % max;
	}
	IC 	s32		randI(s32 min, s32 max)
	{
		return min + randI(max - min);
	}
	IC 	s32		randIs(s32 range)
	{
		return randI(-range, range);
	}
	IC 	s32		randIs(s32 range, s32 offs)
	{
		return offs + randIs(range);
	}

	IC 	F32	maxF()
	{
		return 32767.0f;
	}
	IC 	F32	randF()
	{
		return F32(randI()) / maxF();
	}
	IC 	F32	randF(F32 max)
	{
		return randF() * max;
	}
	IC 	F32	randF(F32 min, F32 max)
	{
		return min + randF(max - min);
	}
	IC 	F32	randFs(F32 range)
	{
		return randF(-range, range);
	}
	IC 	F32	randFs(F32 range, F32 offs)
	{
		return offs + randFs(range);
	}
};

XRCORE_API extern CRandom	Random;
