#pragma once

class CRandom
{
private:
	volatile s32						holdrand;

public:
					CRandom				( ) : holdrand(1)
	{ }
					CRandom				(s32 _seed) : holdrand(_seed)
	{ }

	IC void			seed				(s32 val)
	{
		holdrand						= val;
	}
	IC s32			maxI				( )
	{
		return 32767;
	}

	ICN s32			randI				( )
	{
		return(((holdrand = holdrand * 214013L + 2531011L) >> 16) & 0x7fff);
	}
	IC s32			randI				(s32 max)
	{
		VERIFY							(max);
		return (randI( ) % max);
	}
	IC s32			randI				(s32 min, s32 max)
	{
		return (min + randI(max - min));
	}
	IC s32			randIs				(s32 range)
	{
		return randI(-range, range);
	}
	IC s32			randIs				(s32 range, s32 offs)
	{
		return (offs + randIs(range));
	}

	IC f32			maxF				( )
	{
		return 32767.0f;
	}
	IC f32			randF				( )
	{
		return (f32(randI( )) / maxF( ));
	}
	IC f32			randF				(f32 max)
	{
		return (randF( ) * max);
	}
	IC f32			randF				(f32 min, f32 max)
	{
		return (min + randF(max - min));
	}
	IC f32			randFs				(f32 range)
	{
		return randF(-range, range);
	}
	IC f32			randFs				(f32 range, f32 offs)
	{
		return (offs + randFs(range));
	}
};

CORE_API extern	CRandom				Random;
