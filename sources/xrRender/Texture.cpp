// Texture.cpp: implementation of the CTexture class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#pragma warning(disable:4995)
#include <d3dx9.h>
#pragma warning(default:4995)

void fix_texture_name(pstr fn)
{
	pstr _ext = strext(fn);
	if(  _ext					&&
	  (0==stricmp(_ext,".tga")	||
		0==stricmp(_ext,".dds")	||
		0==stricmp(_ext,".bmp")	||
		0==stricmp(_ext,".ogm")	) )
		*_ext = 0;
}

s32 get_texture_load_lod(pcstr fn)
{
	CIniFile::Sect& sect	= pSettings->r_section("reduce_lod_texture_list");
	CIniFile::SectCIt it_	= sect.Data.begin();
	CIniFile::SectCIt it_e_	= sect.Data.end();

	CIniFile::SectCIt it	= it_;
	CIniFile::SectCIt it_e	= it_e_;

	for(;it!=it_e;++it)
	{
		if( strstr(fn, it->first.c_str()) )
		{
			if(psTextureLOD<1)
				return 0;
			else
			if(psTextureLOD<3)
				return 1;
			else
				return 2;
		}
	}

	if(psTextureLOD<2)
		return 0;
	else
	if(psTextureLOD<4)
		return 1;
	else
		return 2;
}

u32 calc_texture_size(int lod, u32 mip_cnt, u32 orig_size)
{
	if(1==mip_cnt)
		return orig_size;

	int _lod		= lod;
	f32 res		= f32(orig_size);

	while(_lod>0){
		--_lod;
		res		-= res/1.333f;
	}
	return iFloor	(res);
}

const f32		_BUMPHEIGH = 8.f;
//////////////////////////////////////////////////////////////////////
// Utility pack
//////////////////////////////////////////////////////////////////////
IC u32 GetPowerOf2Plus1	(u32 v)
{
        u32 cnt=0;
        while (v) {v>>=1; cnt++; };
        return cnt;
}
IC void	Reduce				(int& w, int& h, int& l, int& skip)
{
	while ((l>1) && skip)
	{
		w /= 2;
		h /= 2;
		l -= 1;

		skip--;
	}
	if (w<1)	w=1;
	if (h<1)	h=1;
}

void				TW_Save	(IDirect3DTexture9* T, pcstr name, pcstr prefix, pcstr postfix)
{
	string256		fn;		strconcat	(sizeof(fn),fn,name,"_",prefix,"-",postfix);
	for (int it=0; it<int(xr_strlen(fn)); it++)	
		if ('\\'==fn[it])	fn[it]	= '_';
	string256		fn2;	strconcat	(sizeof(fn2),fn2,"debug\\",fn,".dds");
	Log						("* debug texture save: ",fn2);
	R_CHK					(D3DXSaveTextureToFile	(fn2,D3DXIFF_DDS,T,0));
}

IDirect3DTexture9*	TW_LoadTextureFromTexture
(
	IDirect3DTexture9*		t_from,
	D3DFORMAT&				t_dest_fmt,
	int						levels_2_skip,
	u32&					w,
	u32&					h
)
{
	// Calculate levels & dimensions
	IDirect3DTexture9*		t_dest			= NULL;
	D3DSURFACE_DESC			t_from_desc0	;
	R_CHK					(t_from->GetLevelDesc	(0,&t_from_desc0));
	int levels_exist		= t_from->GetLevelCount();
	int top_width			= t_from_desc0.Width;
	int top_height			= t_from_desc0.Height;
	Reduce					(top_width,top_height,levels_exist,levels_2_skip);

	// Create HW-surface
	if (D3DX_DEFAULT==t_dest_fmt)	t_dest_fmt = t_from_desc0.Format;
	R_CHK					(D3DXCreateTexture(
		HW.pDevice,
		top_width,top_height,
		levels_exist,0,t_dest_fmt,
		D3DPOOL_MANAGED,&t_dest
		));

	// Copy surfaces & destroy temporary
	IDirect3DTexture9* T_src= t_from;
	IDirect3DTexture9* T_dst= t_dest;

	int		L_src			= T_src->GetLevelCount	()-1;
	int		L_dst			= T_dst->GetLevelCount	()-1;
	for (; L_dst>=0; L_src--,L_dst--)
	{
		// Get surfaces
		IDirect3DSurface9		*S_src, *S_dst;
		R_CHK	(T_src->GetSurfaceLevel	(L_src,&S_src));
		R_CHK	(T_dst->GetSurfaceLevel	(L_dst,&S_dst));

		// Copy
		R_CHK	(D3DXLoadSurfaceFromSurface(S_dst,NULL,NULL,S_src,NULL,NULL,D3DX_FILTER_NONE,0));

		// Release surfaces
		_RELEASE				(S_src);
		_RELEASE				(S_dst);
	}

	// OK
	w						= top_width;
	h						= top_height;
	return					t_dest;
}

template	<class _It>
IC	void	TW_Iterate_1OP
(
	IDirect3DTexture9*		t_dst,
	IDirect3DTexture9*		t_src,
	const _It				pred
)
{
	DWORD mips							= t_dst->GetLevelCount();
	R_ASSERT							(mips == t_src->GetLevelCount());
	for (DWORD i = 0; i < mips; i++)	{
		D3DLOCKED_RECT				Rsrc,Rdst;
		D3DSURFACE_DESC				desc,descS;

		t_dst->GetLevelDesc			(i, &desc);
		t_src->GetLevelDesc			(i, &descS);
		VERIFY						(desc.Format==descS.Format);
		VERIFY						(desc.Format==D3DFMT_A8R8G8B8);
		t_src->LockRect				(i,&Rsrc,0,0);
		t_dst->LockRect				(i,&Rdst,0,0);
		for (u32 y = 0; y < desc.Height; y++)	{
			for (u32 x = 0; x < desc.Width; x++)	{
				DWORD&	pSrc	= *(((DWORD*)((BYTE*)Rsrc.pBits + (y * Rsrc.Pitch)))+x);
				DWORD&	pDst	= *(((DWORD*)((BYTE*)Rdst.pBits + (y * Rdst.Pitch)))+x);
				pDst			= pred(pDst,pSrc);
			}
		}
		t_dst->UnlockRect			(i);
		t_src->UnlockRect			(i);
	}
}
template	<class _It>
IC	void	TW_Iterate_2OP
(
	IDirect3DTexture9*		t_dst,
	IDirect3DTexture9*		t_src0,
	IDirect3DTexture9*		t_src1,
	const _It				pred
 )
{
	DWORD mips							= t_dst->GetLevelCount();
	R_ASSERT							(mips == t_src0->GetLevelCount());
	R_ASSERT							(mips == t_src1->GetLevelCount());
	for (DWORD i = 0; i < mips; i++)	{
		D3DLOCKED_RECT				Rsrc0,Rsrc1,Rdst;
		D3DSURFACE_DESC				desc,descS0,descS1;

		t_dst->GetLevelDesc			(i, &desc);
		t_src0->GetLevelDesc		(i, &descS0);
		t_src1->GetLevelDesc		(i, &descS1);
		VERIFY						(desc.Format==descS0.Format);
		VERIFY						(desc.Format==descS1.Format);
		VERIFY						(desc.Format==D3DFMT_A8R8G8B8);
		t_src0->LockRect			(i,&Rsrc0,	0,0);
		t_src1->LockRect			(i,&Rsrc1,	0,0);
		t_dst->LockRect				(i,&Rdst,	0,0);
		for (u32 y = 0; y < desc.Height; y++)	{
			for (u32 x = 0; x < desc.Width; x++)	{
				DWORD&	pSrc0	= *(((DWORD*)((BYTE*)Rsrc0.pBits + (y * Rsrc0.Pitch)))+x);
				DWORD&	pSrc1	= *(((DWORD*)((BYTE*)Rsrc1.pBits + (y * Rsrc1.Pitch)))+x);
				DWORD&	pDst	= *(((DWORD*)((BYTE*)Rdst.pBits  + (y * Rdst.Pitch)))+x);
				pDst			= pred(pDst,pSrc0,pSrc1);
			}
		}
		t_dst->UnlockRect			(i);
		t_src0->UnlockRect			(i);
		t_src1->UnlockRect			(i);
	}
}

IC u32 it_gloss_rev		(u32 d, u32 s)	{	return	color_rgba	(
	color_get_A(s),		// gloss
	color_get_B(d),
	color_get_G(d),
	color_get_R(d)		);
}
IC u32 it_gloss_rev_base(u32 d, u32 s)	{	
	u32		occ		= color_get_A(d)/3;
	u32		def		= 8;
	u32		gloss	= (occ*1+def*3)/4;
	return	color_rgba	(
		gloss,			// gloss
		color_get_B(d),
		color_get_G(d),
		color_get_R(d)
	);
}
IC u32 it_difference	(u32 d, u32 orig, u32 ucomp)	{	return	color_rgba(
	128+(int(color_get_R(orig))-int(color_get_R(ucomp)))*2,		// R-error
	128+(int(color_get_G(orig))-int(color_get_G(ucomp)))*2,		// G-error
	128+(int(color_get_B(orig))-int(color_get_B(ucomp)))*2,		// B-error
	128+(int(color_get_A(orig))-int(color_get_A(ucomp)))*2	);	// A-error	
}
IC u32 it_height_rev	(u32 d, u32 s)	{	return	color_rgba	(
	color_get_A(d),					// diff x
	color_get_B(d),					// diff y
	color_get_G(d),					// diff z
	color_get_R(s)	);				// height
}
IC u32 it_height_rev_base(u32 d, u32 s)	{	return	color_rgba	(
	color_get_A(d),					// diff x
	color_get_B(d),					// diff y
	color_get_G(d),					// diff z
	(color_get_R(s)+color_get_G(s)+color_get_B(s))/3	);	// height
}

IDirect3DBaseTexture9*	CRender::texture_load(pcstr fRName, u32& ret_msize)
{
	IDirect3DTexture9*		pTexture2D		= NULL;
	IDirect3DCubeTexture9*	pTextureCUBE	= NULL;
	string_path				fn;
	u32						dwWidth,dwHeight;
	u32						img_size		= 0;
	int						img_loaded_lod	= 0;
	D3DFORMAT				fmt;
	u32						mip_cnt=u32(-1);
	// validation
	R_ASSERT				(fRName);
	R_ASSERT				(fRName[0]);

	// make file name
	string_path				fname;
	strcpy(fname,fRName); //. andy if (strext(fname)) *strext(fname)=0;
	fix_texture_name		(fname);
	IReader* S				= NULL;
	//if (FS.exist(fn,"$game_textures$",fname,	".dds")	&& strstr(fname,"_bump"))	goto _BUMP;
	if (!FS.exist(fn,"$game_textures$",	fname,	".dds")	&& strstr(fname,"_bump"))	goto _BUMP_from_base;
	if (FS.exist(fn,"$level$",			fname,	".dds"))							goto _DDS;
	if (FS.exist(fn,"$game_saves$",		fname,	".dds"))							goto _DDS;
	if (FS.exist(fn,"$game_textures$",	fname,	".dds"))							goto _DDS;

	Debug.fatal(DEBUG_INFO,"Can't find texture '%s'",fname);

	return 0;

_DDS:
	{
		// Load and get header
		D3DXIMAGE_INFO			IMG;
		S						= FS.r_open	(fn);
		img_size				= S->length	();

#ifdef LOADED_TEXTURE_LOG
		Msg("* Loaded texture: [%s] %i byte", fn, S->length( ));
#endif // def LOADED_TEXTURE_LOG

		R_ASSERT				(S);
		R_CHK2					(D3DXGetImageInfoFromFileInMemory	(S->pointer(),S->length(),&IMG), fn);
		if (IMG.ResourceType	== D3DRTYPE_CUBETEXTURE)			goto _DDS_CUBE;
		else														goto _DDS_2D;

_DDS_CUBE:
		{
			R_CHK(D3DXCreateCubeTextureFromFileInMemoryEx(
				HW.pDevice,
				S->pointer(),S->length(),
				D3DX_DEFAULT,
				IMG.MipLevels,0,
				IMG.Format,
				D3DPOOL_MANAGED,
				D3DX_DEFAULT,
				D3DX_DEFAULT,
				0,&IMG,0,
				&pTextureCUBE
				));
			FS.r_close				(S);

			// OK
			dwWidth					= IMG.Width;
			dwHeight				= IMG.Height;
			fmt						= IMG.Format;
			ret_msize				= calc_texture_size(img_loaded_lod, mip_cnt, img_size);
			mip_cnt					= pTextureCUBE->GetLevelCount();
			return					pTextureCUBE;
		}
_DDS_2D:
		{
			// Check for LMAP and compress if needed
			strlwr					(fn);


			// Load   SYS-MEM-surface, bound to device restrictions
			IDirect3DTexture9*		T_sysmem;
			R_CHK2(D3DXCreateTextureFromFileInMemoryEx
					(
						HW.pDevice,S->pointer(),S->length(),
						D3DX_DEFAULT,D3DX_DEFAULT,
						IMG.MipLevels,0,
						IMG.Format,
						D3DPOOL_SYSTEMMEM,
						D3DX_DEFAULT,
						D3DX_DEFAULT,
						0,&IMG,0,
						&T_sysmem
					), fn);
			FS.r_close				(S);
			img_loaded_lod			= get_texture_load_lod(fn);
			pTexture2D				= TW_LoadTextureFromTexture(T_sysmem,IMG.Format, img_loaded_lod, dwWidth, dwHeight);
			mip_cnt					= pTexture2D->GetLevelCount();
			_RELEASE				(T_sysmem);

			// OK
			fmt						= IMG.Format;
			ret_msize				= calc_texture_size(img_loaded_lod, mip_cnt, img_size);
			return					pTexture2D;
		}
	}
	/*
_BUMP:
	{
		// Load   SYS-MEM-surface, bound to device restrictions
		D3DXIMAGE_INFO			IMG;
		IReader* S				= FS.r_open	(fn);
		msize					= S->length	();
		IDirect3DTexture9*		T_height_gloss;
		R_CHK(D3DXCreateTextureFromFileInMemoryEx(
			HW.pDevice,	S->pointer(),S->length(),
			D3DX_DEFAULT,D3DX_DEFAULT,	D3DX_DEFAULT,0,D3DFMT_A8R8G8B8,
			D3DPOOL_SYSTEMMEM,			D3DX_DEFAULT,D3DX_DEFAULT,
			0,&IMG,0,&T_height_gloss	));
		FS.r_close				(S);
		//TW_Save						(T_height_gloss,fname,"debug-0","original");

		// Create HW-surface, compute normal map
		IDirect3DTexture9*	T_normal_1	= 0;
		R_CHK(D3DXCreateTexture		(HW.pDevice,IMG.Width,IMG.Height,D3DX_DEFAULT,0,D3DFMT_A8R8G8B8,D3DPOOL_SYSTEMMEM,&T_normal_1));
		R_CHK(D3DXComputeNormalMap	(T_normal_1,T_height_gloss,0,0,D3DX_CHANNEL_RED,_BUMPHEIGH));
		//TW_Save						(T_normal_1,fname,"debug-1","normal");

		// Transfer gloss-map
		TW_Iterate_1OP				(T_normal_1,T_height_gloss,it_gloss_rev);
		//TW_Save						(T_normal_1,fname,"debug-2","normal-G");

		// Compress
		fmt								= D3DFMT_DXT5;
		IDirect3DTexture9*	T_normal_1C	= TW_LoadTextureFromTexture(T_normal_1,fmt,psTextureLOD,dwWidth,dwHeight);
		//TW_Save						(T_normal_1C,fname,"debug-3","normal-G-C");
		
#if RENDER==R_R2
		// Decompress (back)
		fmt								= D3DFMT_A8R8G8B8;
		IDirect3DTexture9*	T_normal_1U	= TW_LoadTextureFromTexture(T_normal_1C,fmt,0,dwWidth,dwHeight);
		// TW_Save						(T_normal_1U,fname,"debug-4","normal-G-CU");

		// Calculate difference
		IDirect3DTexture9*	T_normal_1D = 0;
		R_CHK(D3DXCreateTexture(HW.pDevice,dwWidth,dwHeight,T_normal_1U->GetLevelCount(),0,D3DFMT_A8R8G8B8,D3DPOOL_SYSTEMMEM,&T_normal_1D));
		TW_Iterate_2OP				(T_normal_1D,T_normal_1,T_normal_1U,it_difference);
		// TW_Save						(T_normal_1D,fname,"debug-5","normal-G-diff");

		// Reverse channels back + transfer heightmap
		TW_Iterate_1OP				(T_normal_1D,T_height_gloss,it_height_rev);
		// TW_Save						(T_normal_1D,fname,"debug-6","normal-G-diff-H");

		// Compress
		fmt								= D3DFMT_DXT5;
		IDirect3DTexture9*	T_normal_2C	= TW_LoadTextureFromTexture(T_normal_1D,fmt,0,dwWidth,dwHeight);
		// TW_Save						(T_normal_2C,fname,"debug-7","normal-G-diff-H-C");
		_RELEASE					(T_normal_1U	);
		_RELEASE					(T_normal_1D	);

		// 
		string256			fnameB;
		strconcat			(fnameB,"$user$",fname,"X");
		ref_texture			t_temp		= Device.Resources->_CreateTexture	(fnameB);
		t_temp->surface_set	(T_normal_2C	);
		_RELEASE			(T_normal_2C	);	// texture should keep reference to it by itself
#endif

		// release and return
		// T_normal_1C	- normal.gloss,		reversed
		// T_normal_2C	- 2*error.height,	non-reversed
		_RELEASE			(T_height_gloss	);
		_RELEASE			(T_normal_1		);
		return				T_normal_1C;
	}
	*/
_BUMP_from_base:
	{
		Msg			("! auto-generated bump map: %s",fname);
//////////////////
		if (strstr(fname,"_bump#"))			
		{
			R_ASSERT2	(FS.exist(fn,"$game_textures$",	"ed\\ed_dummy_bump#",	".dds"), "ed_dummy_bump#");
			S						= FS.r_open	(fn);
			R_ASSERT2				(S, fn);
			img_size				= S->length	();
			goto		_DDS_2D;
		}
		if (strstr(fname,"_bump"))			
		{
			R_ASSERT2	(FS.exist(fn,"$game_textures$",	"ed\\ed_dummy_bump",	".dds"),"ed_dummy_bump");
			S						= FS.r_open	(fn);

			R_ASSERT2	(S, fn);

			img_size				= S->length	();
			goto		_DDS_2D;
		}
//////////////////

		*strstr		(fname,"_bump")	= 0;
		R_ASSERT2	(FS.exist(fn,"$game_textures$",	fname,	".dds"),fname);

		// Load   SYS-MEM-surface, bound to device restrictions
		D3DXIMAGE_INFO			IMG;
		S						= FS.r_open	(fn);
		img_size				= S->length	();
		IDirect3DTexture9*		T_base;
		R_CHK2(D3DXCreateTextureFromFileInMemoryEx(
			HW.pDevice,	S->pointer(),S->length(),
			D3DX_DEFAULT,D3DX_DEFAULT,	D3DX_DEFAULT,0,D3DFMT_A8R8G8B8,
			D3DPOOL_SYSTEMMEM,			D3DX_DEFAULT,D3DX_DEFAULT,
			0,&IMG,0,&T_base	), fn);
		FS.r_close				(S);

		// Create HW-surface
		IDirect3DTexture9*	T_normal_1	= 0;
		R_CHK(D3DXCreateTexture		(HW.pDevice,IMG.Width,IMG.Height,D3DX_DEFAULT,0,D3DFMT_A8R8G8B8,D3DPOOL_SYSTEMMEM, &T_normal_1));
		R_CHK(D3DXComputeNormalMap	(T_normal_1,T_base,0,D3DX_NORMALMAP_COMPUTE_OCCLUSION,D3DX_CHANNEL_LUMINANCE,_BUMPHEIGH));

		// Transfer gloss-map
		TW_Iterate_1OP				(T_normal_1,T_base,it_gloss_rev_base);

		// Compress
		fmt								= D3DFMT_DXT5;
		img_loaded_lod					= get_texture_load_lod(fn);
		IDirect3DTexture9*	T_normal_1C	= TW_LoadTextureFromTexture(T_normal_1, fmt, img_loaded_lod, dwWidth, dwHeight);
		mip_cnt							= T_normal_1C->GetLevelCount();

#if RENDER==R_R2	
		// Decompress (back)
		fmt								= D3DFMT_A8R8G8B8;
		IDirect3DTexture9*	T_normal_1U	= TW_LoadTextureFromTexture(T_normal_1C,fmt,0,dwWidth,dwHeight);

		// Calculate difference
		IDirect3DTexture9*	T_normal_1D = 0;
		R_CHK(D3DXCreateTexture(HW.pDevice,dwWidth,dwHeight,T_normal_1U->GetLevelCount(),0,D3DFMT_A8R8G8B8,D3DPOOL_SYSTEMMEM,&T_normal_1D));
		TW_Iterate_2OP		(T_normal_1D,T_normal_1,T_normal_1U,it_difference);

		// Reverse channels back + transfer heightmap
		TW_Iterate_1OP		(T_normal_1D,T_base,it_height_rev_base);

		// Compress
		fmt								= D3DFMT_DXT5;
		IDirect3DTexture9*	T_normal_2C	= TW_LoadTextureFromTexture(T_normal_1D,fmt,0,dwWidth,dwHeight);
		_RELEASE						(T_normal_1U	);
		_RELEASE						(T_normal_1D	);

		// 
		string256			fnameB;
		strconcat			(sizeof(fnameB),fnameB,"$user$",fname,"_bumpX");
		ref_texture			t_temp			= Device.Resources->_CreateTexture	(fnameB);
		t_temp->surface_set	(T_normal_2C	);
		_RELEASE			(T_normal_2C	);	// texture should keep reference to it by itself
#endif
		// T_normal_1C	- normal.gloss,		reversed
		// T_normal_2C	- 2*error.height,	non-reversed
		_RELEASE			(T_base);
		_RELEASE			(T_normal_1);
		ret_msize			= calc_texture_size(img_loaded_lod, mip_cnt, img_size);
		return				T_normal_1C;
	}
}
