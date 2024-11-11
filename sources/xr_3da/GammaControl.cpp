#include "stdafx.h"
#include "GammaControl.h"

IC u16 clr2gamma(f32 c)
{
	s32 C								= iFloor(c);
	clamp								(C, 0, 65535);
	return u16(C);
}

void CGammaControl::Update( )
{
	if (HW.pDevice)
	{
		D3DGAMMARAMP					G;
		GenLUT							(G);
		HW.pDevice->SetGammaRamp		(0, D3DSGR_NO_CALIBRATION, &G);
	}
}

void CGammaControl::GenLUT(D3DGAMMARAMP& G)
{
	f32 og								= 1.0f / (fGamma + EPSILON_5);
	f32 B								= fBrightness / 2.0f;
	f32 C								= fContrast / 2.0f;
	for (s32 i = 0; i < 256; i++)
	{
		f32 c							= (C + 0.5f) * powf(i / 255.0f, og) * 65535.0f + (B - 0.5f) * 32768.0f - C * 32768.0f + 16384.0f;
		G.red[i]						= clr2gamma(c * cBalance.r);
		G.green[i]						= clr2gamma(c * cBalance.g);
		G.blue[i]						= clr2gamma(c * cBalance.b);
	}
}
