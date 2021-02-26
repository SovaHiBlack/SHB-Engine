#pragma once

class CRandom
{
private:
	volatile	int		holdrand;
public:
	CRandom()			: holdrand(1)				{};
	CRandom(int _seed)	: holdrand(_seed)			{};

	IC 	void	seed	(int val)					{ holdrand=val;	}
	IC 	int		maxI	()							{ return 32767;	}

	__declspec (noinline)	int		randI	()							{ return(((holdrand = holdrand * 214013L + 2531011L) >> 16) & 0x7fff); }
	IC 	int		randI	(int max)					{ VERIFY(max);  return randI()%max; }
	IC 	int		randI	(int min, int max)			{ return min+randI(max-min); }
	IC 	int		randIs	(int range)					{ return randI(-range,range); }
	IC 	int		randIs	(int range, int offs)		{ return offs+randIs(range); }

	IC 	float	maxF	()							{ return 32767.f;	}
	IC 	float	randF	()							{ return float(randI())/maxF();	}
	IC 	float	randF	(float max)					{ return randF()*max; }
	IC 	float	randF	(float min,float max)		{ return min+randF(max-min); }
	IC 	float	randFs	(float range)				{ return randF(-range,range); }
	IC 	float	randFs	(float range, float offs)	{ return offs+randFs(range); }
};

CORE_API extern CRandom	Random;
