#include "stdafx.h"

BOOL	xrRender_test_hw		()
{
	D3DCAPS9					caps	;
	CHW							_HW;
	_HW.CreateD3D				()		;
	_HW.pD3D->GetDeviceCaps		(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,&caps);
	_HW.DestroyD3D				()		;
	unsigned short		ps_ver_major		= unsigned short( u32(u32(caps.PixelShaderVersion)&u32(0xf << 8ul))>>8 );
	unsigned short		ps_instructions		= unsigned short(caps.PS20Caps.NumInstructionSlots);
	unsigned short		mrt_count			= unsigned short(caps.NumSimultaneousRTs);
	if		(ps_ver_major<2)		return	FALSE;
	if		(ps_instructions<256)	return	FALSE;
	if		(mrt_count<3)			return	FALSE;
	return	TRUE;
}
