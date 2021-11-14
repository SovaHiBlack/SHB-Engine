#include "stdafx.h"

#include "GammaControl.h"

inline unsigned short clr2gamma(float c)
{
	int C							= iFloor(c);
	clamp							(C, 0, 65535);
	return unsigned short(C);
}

void CGammaControl::Update( )
{
	if (HW.pDevice)
	{
		D3DGAMMARAMP				G;
		GenLUT						(G);
		HW.pDevice->SetGammaRamp	(0, D3DSGR_NO_CALIBRATION, &G);
	}
}

void CGammaControl::GenLUT(D3DGAMMARAMP& G)
{
	float og						= 1.0f / (fGamma + EPS);
	float B							= fBrightness / 2.0f;
	float C							= fContrast / 2.0f;
	for (int i = 0; i < 256; i++)
	{
		float c						= (C + 0.5f) * powf(i / 255.0f, og) * 65535.0f + (B - 0.5f) * 32768.0f - C * 32768.0f + 16384.0f;
		G.red[i]					= clr2gamma(c * cBalance.r);
		G.green[i]					= clr2gamma(c * cBalance.g);
		G.blue[i]					= clr2gamma(c * cBalance.b);
	}
}
