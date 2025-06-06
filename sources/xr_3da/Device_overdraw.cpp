#include "stdafx.h"

void CRenderDevice::overdrawBegin( )
{
	// Turn stenciling
	CHK_DX(HW.pDevice->SetRenderState(D3DRS_STENCILENABLE, TRUE));
	CHK_DX(HW.pDevice->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS));
	CHK_DX(HW.pDevice->SetRenderState(D3DRS_STENCILREF, 0));
	CHK_DX(HW.pDevice->SetRenderState(D3DRS_STENCILMASK, 0x00000000));
	CHK_DX(HW.pDevice->SetRenderState(D3DRS_STENCILWRITEMASK, 0xffffffff));

	// Increment the stencil buffer for each pixel drawn
	CHK_DX(HW.pDevice->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP));
	CHK_DX(HW.pDevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_INCRSAT));

	if (1 == HW.Caps.SceneMode)
	{
		CHK_DX(HW.pDevice->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP));
	}	// Overdraw
	else
	{
		CHK_DX(HW.pDevice->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_INCRSAT));
	}	// ZB access
}

void CRenderDevice::overdrawEnd( )
{
	// Set up the stencil states
	CHK_DX(HW.pDevice->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP));
	CHK_DX(HW.pDevice->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP));
	CHK_DX(HW.pDevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP));
	CHK_DX(HW.pDevice->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_EQUAL));
	CHK_DX(HW.pDevice->SetRenderState(D3DRS_STENCILMASK, 0xff));

	// Set the background to black
	CHK_DX(HW.pDevice->Clear(0, 0, D3DCLEAR_TARGET, D3DCOLOR_XRGB(255, 0, 0), 0, 0));

	// Draw a rectangle wherever the count equal I
	RCache.OnFrameEnd( );
	CHK_DX(HW.pDevice->SetFVF(FVF::F_TL));

	// Render gradients
	for (s32 I = 0; I < 12; I++)
	{
		u32	_c = I * 256 / 13;
		u32	c = D3DCOLOR_XRGB(_c, _c, _c);

		FVF::TL	pv[4];
		pv[0].set(f32(0), f32(dwHeight), c, 0, 0);
		pv[1].set(f32(0), f32(0), c, 0, 0);
		pv[2].set(f32(dwWidth), f32(dwHeight), c, 0, 0);
		pv[3].set(f32(dwWidth), f32(0), c, 0, 0);

		CHK_DX(HW.pDevice->SetRenderState(D3DRS_STENCILREF, I));
		CHK_DX(HW.pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, pv, sizeof(FVF::TL)));
	}

	CHK_DX(HW.pDevice->SetRenderState(D3DRS_STENCILENABLE, FALSE));
}
