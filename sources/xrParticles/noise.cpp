#include "stdafx.h"

#include "noise.h"
 
//==============================================================================
// Perlin's noise from Texturing and Modeling...
#define B 256

#define DOT(a,b) ( a[0]*b[0] + a[1]*b[1] + a[2]*b[2])
#define AT(rx,ry,rz)	(rx*q[0] + ry*q[1] + rz*q[2] );
#define S_CURVE(t)		( t*t*(3.f-2.f*t) )
#define LERP(t, a, b)	( a + t*(b-a) )
#define PN_SETUP(i,b0,b1,r0,r1) \
	t = vec[i] + 10000.f;\
	b0 = iFloor(t) & (B-1);\
	b1 = (b0+1) & (B-1);\
	r0 = t - iFloor(t);\
	r1 = r0 - 1.f;

static int		start = 1;
static int 		p[B+B+2];
static f32	g[B+B+2][3];

void	noise3Init();

//--------------------------------------------------------------------
f32	noise3(const fVector3& vec)
{
	int		bx0, bx1;
	int		by0, by1;
	int		bz0, bz1;
	int		b00, b10, b01, b11;
	f32	rx0;
	f32 rx1;
	f32	ry0;
	f32 ry1;
	f32	rz0;
	f32 rz1;
	f32* q;
	f32	sx;
	f32 sy;
	f32 sz;
	f32 a;
	f32 b;
	f32 c;
	f32 d;
	f32 t;
	f32 u;
	f32 v;
	int		i, j;
	
	if (start)
	{
		start = 0;
		noise3Init();
	}
	
	PN_SETUP(0, bx0, bx1, rx0, rx1);
	PN_SETUP(1, by0, by1, ry0, ry1);
	PN_SETUP(2, bz0, bz1, rz0, rz1);
	
	i = p[bx0];
	j = p[bx1];
	
	b00 = p[ i+by0 ];
	b10 = p[ j+by0 ];
	b01 = p[ i+by1 ];
	b11 = p[ j+by1 ];
	
	sx = S_CURVE(rx0);
	sy = S_CURVE(ry0);
	sz = S_CURVE(rz0);
	
	q = g[ b00+bz0 ]; u = AT(rx0, ry0, rz0);
	q = g[ b10+bz0 ]; v = AT(rx1, ry0, rz0);
	a = LERP( sx, u, v );
	
	q = g[ b01+bz0 ]; u = AT(rx0, ry1, rz0);
	q = g[ b11+bz0 ]; v = AT(rx1, ry1, rz0);
	b = LERP( sx, u, v );
	
	c = LERP(sy, a, b);
	
	q = g[ b00+bz1 ]; u = AT(rx0, ry0, rz1);
	q = g[ b10+bz1 ]; v = AT(rx1, ry0, rz1);
	a = LERP( sx, u, v );
	
	q = g[ b01+bz1 ]; u = AT(rx0, ry1, rz1);
	q = g[ b11+bz1 ]; v = AT(rx1, ry1, rz1);
	b = LERP( sx, u, v );
	
	d = LERP(sy, a, b);
	
	return 1.5f * LERP(sz, c, d);
}
	
//--------------------------------------------------------------------
void	noise3Init()
{
	int		i, j, k;
	f32	v[3];
	f32 s;
	int	rnd;
	
	srand(1);
	
	for(i = 0; i < B; i++ )
	{
		do
		{
			for(j = 0; j < 3; j++)
			{
				rnd = rand();
				v[j] = f32((rnd % (B+B)) - B) / B;
			}
			s = DOT(v,v);
		} while ( s > 1.0 );
		s = _sqrt(s);
		for (j = 0; j < 3; j++)
			g[i][j] = v[j] / s;
	}
	
	for (i = 0; i < B; i++)
		p[i] = i;
	
	for (i = B; i > 0; i -= 2)
	{
		rnd = rand();
		k = p[i];
		p[i] = p[ (j = rnd%B) ];
		p[j] = k;
	}
	
	for (i = 0; i < B+2; i++)
	{
		p[B+i] = p[i];
		for (j = 0; j < 3; j++)
			g[B+i][j] = g[i][j];
	}
}

//--------------------------------------------------------------------
f32	fractalsum3(const fVector3& v, f32 freq, int octaves)
{
	int		i;
	f32	sum = 0.0;
	fVector3	v_;
	f32	boost = freq;
	v_[0] = v[0]*freq;
	v_[1] = v[1]*freq;
	v_[2] = v[2]*freq;
	
	for (i = 0; i < octaves; i++ )
	{
		sum		+= noise3(v_)/freq;
		freq	*= 2.059f;
		v_[0] = v[0]*freq;
		v_[1] = v[1]*freq;
		v_[2] = v[2]*freq;
	}

	return sum*boost;
}

//--------------------------------------------------------------------
f32	turbulence3(const fVector3& v, f32 freq, int octaves)
{
	int		i;
	f32	sum = 0.0;
	fVector3	v_;
	f32	boost = freq;
	v_[0] = v[0]*freq;
	v_[1] = v[1]*freq;
	v_[2] = v[2]*freq;
	
	for (i = 0; i < octaves; i++ )
	{
		sum		+= _abs(noise3(v_))/freq;
		freq	*= 2.059f;
		v_[0] = v[0]*freq;
		v_[1] = v[1]*freq;
		v_[2] = v[2]*freq;
	}
	return sum*boost;
}

