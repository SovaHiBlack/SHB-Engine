#pragma once
//-----------------------------------------------------------------------------------------------------------
//Gamma control
//-----------------------------------------------------------------------------------------------------------
class	ENGINE_API				CGammaControl
{
	F32						fGamma;
	F32						fBrightness;
	F32						fContrast;
	Fcolor						cBalance;

public:
	CGammaControl		() :
	  fGamma(1.f)
	{ Brightness(1.f); Contrast(1.f); Balance(1.f,1.f,1.f); };

	IC void	Balance		(F32 _r, F32 _g, F32 _b)
	{	cBalance.set	(_r,_g,_b,1);	}
	IC void	Balance		(Fcolor &C)
	{	Balance(C.r,C.g,C.b); }

	IC void Gamma		(F32 G) { fGamma		= G;	}
	IC void Brightness	(F32 B) { fBrightness = B;	}
	IC void Contrast	(F32 C) { fContrast	= C;	}

	void	GetIP		(F32& G, F32& B, F32& C, Fcolor& Balance)
	{
		G			= fGamma;
		B			= fBrightness;
		C			= fContrast;
		Balance.set	(cBalance);
	}

	void	GenLUT		(D3DGAMMARAMP &G);

	void	Update		();
};
