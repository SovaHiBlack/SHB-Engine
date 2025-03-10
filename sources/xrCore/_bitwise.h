#pragma once

// float values defines
#define fdSGN	0x080000000		// mask for sign bit
#define fdMABS  0x07FFFFFFF		// mask for absolute value (~sgn)
#define fdMANT  0x0007FFFFF		// mask for mantissa
#define fdEXPO	0x07F800000		// mask for exponent
#define fdONE	0x03F800000     // 1.0f
#define fdHALF	0x03F000000     // 0.5f
#define fdTWO	0x040000000     // 2.0
#define fdOOB	0x000000000     // "out of bounds" value
#define fdNAN	0x07fffffff     // "Not a number" value
#define fdMAX	0x07F7FFFFF     // FLT_MAX
#define fdRLE10	0x03ede5bdb     // 1/ln10

// integer math on floats
IC BOOL negative(const f32& f)
{
	return (*((unsigned*)(&f)) & fdSGN);
}
IC BOOL positive(const f32& f)
{
	return (*((unsigned*)(&f)) & fdSGN) == 0;
}
IC void set_negative(f32& f)
{
	(*(unsigned*)(&f)) |= fdSGN;
}
IC void set_positive(f32& f)
{
	(*(unsigned*)(&f)) &= ~fdSGN;
}

/*
 * Here are a few nice tricks for 2's complement based machines
 * that I discovered a few months ago.
 */
IC	int		btwLowestBitMask(int v)
{
	return (v & -v);
}
IC	u32		btwLowestBitMask(u32 x)
{
	return x & ~(x - 1);
}

/* Ok, so now we are cooking on gass. Here we use this function for some */
/* rather useful utility functions */
IC	bool	btwIsPow2(int v)
{
	return (btwLowestBitMask(v) == v);
}
IC	bool	btwIsPow2(u32 v)
{
	return (btwLowestBitMask(v) == v);
}

IC	int		btwPow2_Ceil(int v)
{
	int i = btwLowestBitMask(v);
	while (i < v) i <<= 1;
	return i;
}
IC	u32		btwPow2_Ceil(u32 v)
{
	u32 i = btwLowestBitMask(v);
	while (i < v) i <<= 1;
	return i;
}

// Couple more tricks 
// Counting number of nonzero bits for 8bit number: 
IC	u8		btwCount1(u8 v)
{
	v = (v & 0x55) + ((v >> 1) & 0x55);
	v = (v & 0x33) + ((v >> 2) & 0x33);
	return (v & 0x0f) + ((v >> 4) & 0x0f);
}

//same for 32bit 
IC	u32	btwCount1(u32 v)
{
	const u32 g31 = 0x49249249ul;	// = 0100_1001_0010_0100_1001_0010_0100_1001
	const u32 g32 = 0x381c0e07ul;	// = 0011_1000_0001_1100_0000_1110_0000_0111
	v = (v & g31) + ((v >> 1) & g31) + ((v >> 2) & g31);
	v = ((v + (v >> 3)) & g32) + ((v >> 6) & g32);
	return (v + (v >> 9) + (v >> 18) + (v >> 27)) & 0x3f;
}

IC	u64	btwCount1(u64 v)
{
	return btwCount1(u32(v & u32(-1))) + btwCount1(u32(v >> u64(32)));
}

ICF int iFloor(f32 x)
{
	int a = *(const int*)(&x);
	int exponent = (127 + 31) - ((a >> 23) & 0xFF);
	int r = (((u32)(a) << 8) | (1U << 31)) >> exponent;
	exponent += 31 - 127;
	{
		int imask = (!(((((1 << (exponent))) - 1) >> 8) & a));
		exponent -= (31 - 127) + 32;
		exponent >>= 31;
		a >>= 31;
		r -= (imask & a);
		r &= exponent;
		r ^= a;
	}
	return r;
}

/* intCeil() is a non-interesting variant, since effectively
   ceil(x) == -floor(-x)
*/
ICF int iCeil(f32 x)
{
	int a = (*(const int*)(&x));
	int exponent = (127 + 31) - ((a >> 23) & 0xFF);
	int r = (((u32)(a) << 8) | (1U << 31)) >> exponent;
	exponent += 31 - 127;
	{
		int imask = (!(((((1 << (exponent))) - 1) >> 8) & a));
		exponent -= (31 - 127) + 32;
		exponent >>= 31;
		a = ~((a - 1) >> 31);		/* change sign */
		r -= (imask & a);
		r &= exponent;
		r ^= a;
		r = -r;                 /* change sign */
	}
	return r;								/* r = (int)(ceil(f)) */
}

// Validity checks
IC bool fis_gremlin(const f32& f)
{
	u8		value = u8(((*(int*)&f & 0x7f800000) >> 23) - 0x20);
	return	value > 0xc0;
}
IC bool fis_denormal(const f32& f)
{
	return !(*(int*)&f & 0x7f800000);
}

// Approximated calculations
IC f32 apx_InvSqrt(const f32& n)
{
	long tmp = (long(0xBE800000) - *(long*)&n) >> 1;
	f32 y = *(f32*)&tmp;
	return y * (1.47f - 0.47f * n * y * y);
}
// Only for [0..1] (positive) range 
IC f32 apx_asin(const f32 x)
{
	const f32 c1 = 0.892399f;
	const f32 c3 = 1.693204f;
	const f32 c5 = -3.853735f;
	const f32 c7 = 2.838933f;

	const f32 x2 = x * x;
	const f32 d = x * (c1 + x2 * (c3 + x2 * (c5 + x2 * c7)));

	return d;
}
// Only for [0..1] (positive) range 
IC f32 apx_acos(const f32 x)
{
	return PI_DIV_2 - apx_asin(x);
}
