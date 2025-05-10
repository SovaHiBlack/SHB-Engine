#pragma once

#define SAMPLE_SIZE 256

class ENGINE_API CPerlinNoiseCustom
{
protected:
	s32		mSeed;
	bool	mReady;

	s32		p[SAMPLE_SIZE + SAMPLE_SIZE + 2];

protected:
	s32		mOctaves;
	f32	mFrequency;
	f32	mAmplitude;
	xr_vector<f32> mTimes;

public:
	CPerlinNoiseCustom(s32 seed) : mOctaves(2), mFrequency(1), mAmplitude(1), mSeed(seed), mReady(false)
	{ }
	IC void	SetParams(s32 oct, f32 freq, f32 amp)
	{
		mOctaves = oct;
		mFrequency = freq;
		mAmplitude = amp;
	}
	IC void	SetOctaves(s32 oct)
	{
		mOctaves = oct;
		mTimes.resize(mOctaves);
	}
	IC void	SetFrequency(f32 freq)
	{
		mFrequency = freq;
	}
	IC void	SetAmplitude(f32 amp)
	{
		mAmplitude = amp;
	}
};

class ENGINE_API CPerlinNoise1D : public CPerlinNoiseCustom
{
private:
	f32	noise(f32 arg);
	f32	g1[SAMPLE_SIZE + SAMPLE_SIZE + 2];

	void	init( );
	f32	mPrevContiniousTime;

public:
	CPerlinNoise1D(s32 seed) :CPerlinNoiseCustom(seed)
	{
		mPrevContiniousTime = 0.0f;
	}
	f32	Get(f32 x);
	f32	GetContinious(f32 v);
};

class ENGINE_API CPerlinNoise2D : public CPerlinNoiseCustom
{
private:
	f32	noise(const fVector2& vec);
	void	normalize(f32 v[2]);

	f32	g2[SAMPLE_SIZE + SAMPLE_SIZE + 2][2];

	void	init( );

public:
	CPerlinNoise2D(s32 seed) : CPerlinNoiseCustom(seed)
	{ }
	f32	Get(f32 x, f32 y);
};

class ENGINE_API CPerlinNoise3D : public CPerlinNoiseCustom
{
private:
	f32	noise(const fVector3& vec);
	void	normalize(f32 v[3]);

	f32	g3[SAMPLE_SIZE + SAMPLE_SIZE + 2][3];

	void	init( );

public:
	CPerlinNoise3D(s32 seed) : CPerlinNoiseCustom(seed)
	{ }
	f32	Get(f32 x, f32 y, f32 z);
};
