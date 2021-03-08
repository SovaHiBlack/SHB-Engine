#pragma once

class ENGINE_API CGammaControl
{
	float				fGamma;
	float				fBrightness;
	float				fContrast;
	Fcolor				cBalance;

public:
						CGammaControl	( ) : fGamma(1.0f)
	{
		Brightness		(1.0f);
		Contrast		(1.0f);
		Balance			(1.0f, 1.0f, 1.0f);
	};

	inline void				Balance			(float _r, float _g, float _b)
	{
		cBalance.set	(_r, _g, _b, 1);
	}
	inline void				Balance			(Fcolor& C)
	{
		Balance			(C.r, C.g, C.b);
	}

	inline void				Gamma			(float G)
	{
		fGamma			= G;
	}
	inline void				Brightness		(float B)
	{
		fBrightness		= B;
	}
	inline void				Contrast		(float C)
	{
		fContrast		= C;
	}

	void				GetIP			(float& G, float& B, float& C, Fcolor& Balance)
	{
		G				= fGamma;
		B				= fBrightness;
		C				= fContrast;
		Balance.set		(cBalance);
	}

	void				GenLUT			(D3DGAMMARAMP& G);

	void				Update			( );
};
