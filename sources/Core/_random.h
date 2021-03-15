#pragma once

class CRandom
{
private:
	volatile int		holdrand;

public:
									CRandom( ) : holdrand(1)
	{ }
									CRandom(int _seed) : holdrand(_seed)
	{ }

	inline void						seed(int val)
	{
		holdrand = val;
	}
	inline int						maxI( )
	{
		return 32767;
	}

	__declspec (noinline) int		randI( )
	{
		return(((holdrand = holdrand * 214013L + 2531011L) >> 16) & 0x7fff);
	}
	inline int						randI(int max)
	{
		VERIFY(max);
		return randI( ) % max;
	}
	inline int						randI(int min, int max)
	{
		return min + randI(max - min);
	}

	inline int						randIs(int range)
	{
		return randI(-range, range);
	}
	inline int						randIs(int range, int offs)
	{
		return offs + randIs(range);
	}

	inline float					maxF( )
	{
		return 32767.0f;
	}

	inline float					randF( )
	{
		return float(randI( )) / maxF( );
	}
	inline float					randF(float max)
	{
		return randF( ) * max;
	}
	inline float					randF(float min, float max)
	{
		return min + randF(max - min);
	}

	inline float					randFs(float range)
	{
		return randF(-range, range);
	}
	inline float					randFs(float range, float offs)
	{
		return offs + randFs(range);
	}
};

CORE_API extern CRandom	Random;
