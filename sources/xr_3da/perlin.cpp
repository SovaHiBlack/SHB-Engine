/* coherent noise function over 1, 2 or 3 dimensions */
/* (copyright Ken CPerlinNoise) */

#include "stdafx.h"

#include "perlin.h"

#define B SAMPLE_SIZE
#define BM (SAMPLE_SIZE-1)

#define N 0x1000
#define NP 12   /* 2^N */
#define NM 0xfff

#define s_curve(t) ( t * t * (3.0f - 2.0f * t) )
#define lerp(t, a, b) ( a + t * (b - a) )

#define setup(i,b0,b1,r0,r1)\
	t = vec[i] + N;\
	b0 = ((int)t) & BM;\
	b1 = (b0+1) & BM;\
	r0 = t - (int)t;\
	r1 = r0 - 1.0f;

//-------------------------------------------------------------------------------------------------
// CPerlinNoise1D
//-------------------------------------------------------------------------------------------------
void CPerlinNoise1D::init()
{
	int i, j, k;

	for (i = 0 ; i < B ; i++)
	{
		p[i] = i;
		g1[i] = (F32)((rand() % (B + B)) - B) / B;
	}

	while (--i)
	{
		k = p[i];
		p[i] = p[j = rand() % B];
		p[j] = k;
	}

	for (i = 0 ; i < B + 2 ; i++)
	{
		p[B + i] = p[i];
		g1[B + i] = g1[i];
	}
}

F32 CPerlinNoise1D::noise(F32 arg)
{
	int bx0, bx1;
	F32 rx0;
	F32 rx1;
	F32 sx;
	F32 t;
	F32 u;
	F32 v;
	F32 vec[1];

	vec[0] = arg;

	if (!mReady){ 
		srand(mSeed);
		mReady = true;
		init();
	}

	setup(0, bx0,bx1, rx0,rx1);

	sx = s_curve(rx0);

	u = rx0 * g1[ p[ bx0 ] ];
	v = rx1 * g1[ p[ bx1 ] ];

	return lerp(sx, u, v);
}

F32 CPerlinNoise1D::Get(F32 v)
{
	F32 result	= 0.0f;
	F32 amp		= mAmplitude;
	v				*=mFrequency;
	for(int i=0; i<mOctaves; i++){
		result		+= noise(v)*amp;
		v			*= 2.0f;
		amp			*= 0.5f;
	}
	return result;
}

F32 CPerlinNoise1D::GetContinious(F32 v)
{
	F32 t_v=v;
	if (mPrevContiniousTime!=0.0f)
	{
		v-=mPrevContiniousTime;
	}
	mPrevContiniousTime=t_v;
	F32 result	= 0.0f;
	F32 amp		= mAmplitude;
	v				*=mFrequency;
	for(int i=0; i<mOctaves; i++){
		F32		octave_time=mTimes[i];
		mTimes[i]	=octave_time+v;
		result		+= noise(octave_time+v)*amp;
		v			*= 2.0f;
		amp			*= 0.5f;
	}
	return result;
}

//-------------------------------------------------------------------------------------------------
// CPerlinNoise2D
//-------------------------------------------------------------------------------------------------
void CPerlinNoise2D::init()
{
	int i, j, k;

	for (i = 0 ; i < B ; i++)
	{
		p[i] = i;
		for (j = 0 ; j < 2 ; j++)
			g2[i][j] = (F32)((rand() % (B + B)) - B) / B;
		normalize(g2[i]);
	}

	while (--i)
	{
		k = p[i];
		p[i] = p[j = rand() % B];
		p[j] = k;
	}

	for (i = 0 ; i < B + 2 ; i++)
	{
		p[B + i] = p[i];
		for (j = 0 ; j < 2 ; j++)
			g2[B + i][j] = g2[i][j];
	}
}

F32 CPerlinNoise2D::noise(const Fvector2& vec)
{
	int bx0, bx1, by0, by1, b00, b10, b01, b11;
	F32 rx0;
	F32 rx1;
	F32 ry0;
	F32 ry1;
	F32* q;
	F32 sx;
	F32 sy;
	F32 a;
	F32 b;
	F32 t;
	F32 u;
	F32 v;
	int i, j;

	if (!mReady){ 
		srand(mSeed);
		mReady = true;
		init();
	}

	setup(0,bx0,bx1,rx0,rx1);
	setup(1,by0,by1,ry0,ry1);

	i = p[bx0];
	j = p[bx1];

	b00 = p[i + by0];
	b10 = p[j + by0];
	b01 = p[i + by1];
	b11 = p[j + by1];

	sx = s_curve(rx0);
	sy = s_curve(ry0);

#define at2(rx,ry) ( rx * q[0] + ry * q[1] )

	q = g2[b00];
	u = at2(rx0,ry0);
	q = g2[b10];
	v = at2(rx1,ry0);
	a = lerp(sx, u, v);

	q = g2[b01];
	u = at2(rx0,ry1);
	q = g2[b11];
	v = at2(rx1,ry1);
	b = lerp(sx, u, v);

	return lerp(sy, a, b);
}

void CPerlinNoise2D::normalize(F32 v[2])
{
	F32 s;

	s = _sqrt(v[0] * v[0] + v[1] * v[1]);
	s = 1.0f/s;
	v[0] = v[0] * s;
	v[1] = v[1] * s;
}

F32 CPerlinNoise2D::Get(F32 x, F32 y)
{
	Fvector2 vec	= {x,y};
	F32 result	= 0.0f;
	F32 amp		= mAmplitude;
	vec[0]			*=mFrequency;
	vec[1]			*=mFrequency;
	for(int i=0; i<mOctaves; i++){
		result		+= noise(vec)*amp;
		vec[0]		*= 2.0f;
		vec[1]		*= 2.0f;
		amp			*= 0.5f;
	}
	return result;
}

//-------------------------------------------------------------------------------------------------
// CPerlinNoise3D
//-------------------------------------------------------------------------------------------------
void CPerlinNoise3D::init()
{
	int i, j, k;

	for (i = 0 ; i < B ; i++)
	{
		p[i] = i;
		for (j = 0 ; j < 3 ; j++)
			g3[i][j] = (F32)((rand() % (B + B)) - B) / B;
		normalize(g3[i]);
	}

	while (--i)
	{
		k = p[i];
		p[i] = p[j = rand() % B];
		p[j] = k;
	}

	for (i = 0 ; i < B + 2 ; i++)
	{
		p[B + i] = p[i];
		for (j = 0 ; j < 3 ; j++)
			g3[B + i][j] = g3[i][j];
	}
}

F32 CPerlinNoise3D::noise(const Fvector3& vec)
{
	int bx0, bx1, by0, by1, bz0, bz1, b00, b10, b01, b11;
	F32 rx0;
	F32 rx1;
	F32 ry0;
	F32 ry1;
	F32 rz0;
	F32 rz1;
	F32* q;
	F32 sy;
	F32 sz;
	F32 a;
	F32 b;
	F32 c;
	F32 d;
	F32 t;
	F32 u;
	F32 v;
	int i, j;

	if (!mReady){ 
		srand(mSeed);
		mReady = true;
		init();
	}

	setup(0, bx0,bx1, rx0,rx1);
	setup(1, by0,by1, ry0,ry1);
	setup(2, bz0,bz1, rz0,rz1);

	i = p[ bx0 ];
	j = p[ bx1 ];

	b00 = p[ i + by0 ];
	b10 = p[ j + by0 ];
	b01 = p[ i + by1 ];
	b11 = p[ j + by1 ];

	t  = s_curve(rx0);
	sy = s_curve(ry0);
	sz = s_curve(rz0);

#define at3(rx,ry,rz) ( rx * q[0] + ry * q[1] + rz * q[2] )

	q = g3[ b00 + bz0 ] ; u = at3(rx0,ry0,rz0);
	q = g3[ b10 + bz0 ] ; v = at3(rx1,ry0,rz0);
	a = lerp(t, u, v);

	q = g3[ b01 + bz0 ] ; u = at3(rx0,ry1,rz0);
	q = g3[ b11 + bz0 ] ; v = at3(rx1,ry1,rz0);
	b = lerp(t, u, v);

	c = lerp(sy, a, b);

	q = g3[ b00 + bz1 ] ; u = at3(rx0,ry0,rz1);
	q = g3[ b10 + bz1 ] ; v = at3(rx1,ry0,rz1);
	a = lerp(t, u, v);

	q = g3[ b01 + bz1 ] ; u = at3(rx0,ry1,rz1);
	q = g3[ b11 + bz1 ] ; v = at3(rx1,ry1,rz1);
	b = lerp(t, u, v);

	d = lerp(sy, a, b);

	return lerp(sz, c, d);
}

void CPerlinNoise3D::normalize(F32 v[3])
{
	F32 s;

	s = _sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
	s = 1.0f/s;

	v[0] = v[0] * s;
	v[1] = v[1] * s;
	v[2] = v[2] * s;
}

F32 CPerlinNoise3D::Get(F32 x, F32 y, F32 z)
{
	Fvector3 vec	= {x,y,z};
	F32 result	= 0.0f;
	F32 amp		= mAmplitude;
	vec[0]			*=mFrequency;
	vec[1]			*=mFrequency;
	vec[2]			*=mFrequency;
	for(int i=0; i<mOctaves; i++){
		result		+= noise(vec)*amp;
		vec[0]		*= 2.0f;
		vec[1]		*= 2.0f;
		vec[2]		*= 2.0f;
		amp			*= 0.5f;
	}
	return result;
}

