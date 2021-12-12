#pragma once

//////////////////////////////////////////////////////////////////////////
class	ENGINE_API	CRT		:	public xr_resource_named	{
public:
	IDirect3DTexture9*	pSurface;
	IDirect3DSurface9*	pRT;
	ref_texture			pTexture;

	unsigned int					dwWidth;
	unsigned int					dwHeight;
	D3DFORMAT			fmt;

	unsigned __int64					_order;

	CRT					();
	~CRT				();

	void				create			(const char* Name, unsigned int w, unsigned int h, D3DFORMAT f);
	void				destroy			();
	void				reset_begin		();
	void				reset_end		();
	inline BOOL				valid			()	{ return !!pTexture; }
};
struct ENGINE_API		resptrcode_crt	: public resptr_base<CRT>
{
	void				create			(const char* Name, unsigned int w, unsigned int h, D3DFORMAT f);
	void				destroy			()	{ _set(NULL);		}
};
typedef	resptr_core<CRT,resptrcode_crt>		ref_rt;

//////////////////////////////////////////////////////////////////////////
class	ENGINE_API	CRTC	:	public xr_resource_named
{
public:
	IDirect3DCubeTexture9*	pSurface;
	IDirect3DSurface9*		pRT[6];
	ref_texture				pTexture;

	unsigned int						dwSize;
	D3DFORMAT				fmt;

	unsigned __int64						_order;

	CRTC					();
	~CRTC					();

	void				create			(const char* name, unsigned int size, D3DFORMAT f);
	void				destroy			();
	void				reset_begin		();
	void				reset_end		();
	inline BOOL				valid			()	{ return !pTexture; }
};
struct ENGINE_API		resptrcode_crtc	: public resptr_base<CRTC>
{
	void				create			(const char* Name, unsigned int size, D3DFORMAT f);
	void				destroy			()	{ _set(NULL);		}
};
typedef	resptr_core<CRTC,resptrcode_crtc>		ref_rtc;
