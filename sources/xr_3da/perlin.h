#pragma once

#define SAMPLE_SIZE 256

class ENGINE_API CPerlinNoiseCustom
{
protected:
	int		mSeed;
	bool	mReady;

	int		p					[SAMPLE_SIZE + SAMPLE_SIZE + 2];
protected:
	int		mOctaves;
	F32	mFrequency;
	F32	mAmplitude;
	xr_vector<F32> mTimes;
public:
			CPerlinNoiseCustom	(int seed):
								mOctaves(2),mFrequency(1),mAmplitude(1),mSeed(seed),mReady(false){}
	IC void	SetParams			(int oct, F32 freq, F32 amp)
	{
		mOctaves				= oct;
		mFrequency				= freq;
		mAmplitude				= amp;
	}
	IC void	SetOctaves			(int oct)	{mOctaves	= oct; mTimes.resize(mOctaves);}
	IC void	SetFrequency		(F32 freq){mFrequency	= freq;}
	IC void	SetAmplitude		(F32 amp)	{mAmplitude	= amp;}
};

class ENGINE_API CPerlinNoise1D: public CPerlinNoiseCustom
{
private:
	F32	noise				(F32 arg);
	F32	g1					[SAMPLE_SIZE + SAMPLE_SIZE + 2];

	void	init				();
	F32	mPrevContiniousTime;
public:
			CPerlinNoise1D		(int seed):CPerlinNoiseCustom(seed){mPrevContiniousTime=0.0f;}
			F32	Get					(F32 x);
			F32	GetContinious		(F32 v);
};

class ENGINE_API CPerlinNoise2D: public CPerlinNoiseCustom
{
private:
	F32	noise				(const Fvector2& vec);
	void	normalize			(F32 v[2]);

	F32	g2					[SAMPLE_SIZE + SAMPLE_SIZE + 2][2];

	void	init				();
public:
			CPerlinNoise2D		(int seed):CPerlinNoiseCustom(seed){}
			F32	Get					(F32 x, F32 y);
};

class ENGINE_API CPerlinNoise3D: public CPerlinNoiseCustom
{
private:
	F32	noise				(const Fvector3& vec);
	void	normalize			(F32 v[3]);

	F32	g3					[SAMPLE_SIZE + SAMPLE_SIZE + 2][3];

	void	init				();
public:
			CPerlinNoise3D		(int seed):CPerlinNoiseCustom(seed){}
			F32	Get					(F32 x, F32 y, F32 z);
};
