// HW.h: interface for the CHW class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include "hwcaps.h"

class ENGINE_API CHW
{
	HINSTANCE 				hD3D9;
public:
	IDirect3D9* 			pD3D;		// D3D
	IDirect3DDevice9*		pDevice;	// render device

	IDirect3DSurface9*		pBaseRT;
	IDirect3DSurface9*		pBaseZB;

	CHWCaps					Caps;

	UINT					DevAdapter;
	D3DDEVTYPE				DevT;
	D3DPRESENT_PARAMETERS	DevPP;

	CHW()
	{
		hD3D9		= NULL;
		pD3D		= NULL;
		pDevice		= NULL;
		pBaseRT		= NULL;
		pBaseZB		= NULL;
	};

	void					CreateD3D				();
	void					DestroyD3D				();
	void					CreateDevice			(HWND hw);
	void					DestroyDevice			();

	void					Reset					(HWND hw);

	void					selectResolution		(unsigned int&dwWidth, unsigned int&dwHeight, BOOL bWindowed);
	D3DFORMAT				selectDepthStencil		(D3DFORMAT);
	unsigned int						selectPresentInterval	();
	unsigned int						selectGPU				();
	unsigned int						selectRefresh			(unsigned int dwWidth, unsigned int dwHeight, D3DFORMAT fmt);
	void					updateWindowProps		(HWND hw);
	BOOL					support					(D3DFORMAT fmt, DWORD type, DWORD usage);

#ifdef DEBUG
	void	Validate(void)	{	VERIFY(pDevice); VERIFY(pD3D); };
#else // DEBUG
	void	Validate(void)	{};
#endif // DEBUG

};

extern ENGINE_API CHW		HW;
