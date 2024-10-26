#pragma once
//-----------------------------------------------------------------------------------------------------------
//Gamma control
//-----------------------------------------------------------------------------------------------------------
class	ENGINE_API				CGammaControl
{
	f32						fGamma;
	f32						fBrightness;
	f32						fContrast;
	fColor						cBalance;

public:
	CGammaControl		() :
	  fGamma(1.f)
	{ Brightness(1.f); Contrast(1.f); Balance(1.f,1.f,1.f); };

	IC void	Balance		(f32 _r, f32 _g, f32 _b)
	{	cBalance.set	(_r,_g,_b,1);	}
	IC void	Balance		(fColor& C)
	{	Balance(C.r,C.g,C.b); }

	IC void Gamma		(f32 G) { fGamma		= G;	}
	IC void Brightness	(f32 B) { fBrightness = B;	}
	IC void Contrast	(f32 C) { fContrast	= C;	}

	void	GetIP		(f32& G, f32& B, f32& C, fColor& Balance)
	{
		G			= fGamma;
		B			= fBrightness;
		C			= fContrast;
		Balance.set	(cBalance);
	}

	void	GenLUT		(D3DGAMMARAMP &G);

	void	Update		();
};
