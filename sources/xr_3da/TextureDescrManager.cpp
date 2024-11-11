#include "stdafx.h"

#include "TextureDescrManager.h"
#include "ETextureParams.h"

// eye-params
f32					r__dtex_range = 50;
class cl_dt_scaler : public R_constant_setup
{
public:
	f32				scale;

	cl_dt_scaler(f32 s) : scale(s)
	{ }
	virtual void setup(R_constant* C)
	{
		RCache.set_c(C, scale, scale, scale, 1 / r__dtex_range);
	}
};

void fix_texture_thm_name(pstr fn)
{
	pstr _ext = strext(fn);
	if (_ext &&
		(0 == stricmp(_ext, ".tga") ||
		 0 == stricmp(_ext, ".thm") ||
		 0 == stricmp(_ext, ".dds") ||
		 0 == stricmp(_ext, ".bmp") ||
		 0 == stricmp(_ext, ".ogm")))
		*_ext = 0;
}

void CTextureDescrManager::LoadLTX( )
{
	string_path fname;
	FS.update_path(fname, "$game_textures$", "textures.ltx");

	if (FS.exist(fname))
	{
		CInifile ini(fname);
		if (ini.section_exist("association"))
		{
			CInifile::Sect& data = ini.r_section("association");
			CInifile::SectCIt I = data.Data.begin( );
			CInifile::SectCIt E = data.Data.end( );
			for (; I != E; ++I)
			{
				const CInifile::Item& item = *I;

				texture_desc& desc = m_texture_details[item.first];
				desc.m_assoc = xr_new<texture_assoc>( );

				string_path				T;
				f32					s;

				s32 res = sscanf(*item.second, "%[^,],%f", T, &s);
				R_ASSERT(res == 2);
				desc.m_assoc->detail_name = T;
				desc.m_assoc->cs = xr_new<cl_dt_scaler>(s);
				desc.m_assoc->usage = 0;
				if (strstr(item.second.c_str( ), "usage[diffuse_or_bump]"))
				{
					desc.m_assoc->usage = (1 << 0) | (1 << 1);
				}
				else
				{
					if (strstr(item.second.c_str( ), "usage[bump]"))
					{
						desc.m_assoc->usage = (1 << 1);
					}
					else if (strstr(item.second.c_str( ), "usage[diffuse]"))
					{
						desc.m_assoc->usage = (1 << 0);
					}
				}
			}
		}

		if (ini.section_exist("specification"))
		{
			CInifile::Sect& sect = ini.r_section("specification");
			for (CInifile::SectCIt I2 = sect.Data.begin( ); I2 != sect.Data.end( ); ++I2)
			{
				const CInifile::Item& item = *I2;

				texture_desc& desc = m_texture_details[item.first];
				desc.m_spec = xr_new<texture_spec>( );

				string_path bmode;
				s32 res = sscanf(item.second.c_str( ), "bump_mode[%[^]]], material[%f]", bmode, &desc.m_spec->m_material);
				R_ASSERT(res == 2);
				if ((bmode[0] == 'u') && (bmode[1] == 's') && (bmode[2] == 'e') && (bmode[3] == ':'))
				{
					// bump-map specified
					desc.m_spec->m_bump_name = bmode + 4;
				}
			}
		}
	}
}

void CTextureDescrManager::LoadTHM( )
{
	FS_FileSet flist;
	FS.file_list(flist, "$game_textures$", FS_ListFiles, "*.thm");
	Msg("count of .thm files=%d", flist.size( ));
	FS_FileSetIt It = flist.begin( );
	FS_FileSetIt It_e = flist.end( );
	STextureParams tp;
	string_path fn;
	for (; It != It_e; ++It)
	{
		FS.update_path(fn, "$game_textures$", (*It).name.c_str( ));
		IReader* F = FS.r_open(fn);
		strcpy_s(fn, (*It).name.c_str( ));
		fix_texture_thm_name(fn);

		R_ASSERT(F->find_chunk(THM_CHUNK_TYPE));
		F->r_u32( );
		tp.Clear( );
		tp.Load(*F);
		FS.r_close(F);

		if (STextureParams::ttImage == tp.fmt ||
			STextureParams::ttTerrain == tp.fmt ||
			STextureParams::ttNormalMap == tp.fmt)
		{
			texture_desc& desc = m_texture_details[fn];
			if (tp.detail_name.size( ) && tp.flags.is_any(STextureParams::flDiffuseDetail | STextureParams::flBumpDetail))
			{
				if (desc.m_assoc)
				{
					xr_delete(desc.m_assoc);
				}

				desc.m_assoc = xr_new<texture_assoc>( );
				desc.m_assoc->detail_name = tp.detail_name;
				desc.m_assoc->cs = xr_new<cl_dt_scaler>(tp.detail_scale);
				desc.m_assoc->usage = 0;

				if (tp.flags.is(STextureParams::flDiffuseDetail))
				{
					desc.m_assoc->usage |= (1 << 0);
				}

				if (tp.flags.is(STextureParams::flBumpDetail))
				{
					desc.m_assoc->usage |= (1 << 1);
				}
			}

			if (desc.m_spec)
			{
				xr_delete(desc.m_spec);
			}

			desc.m_spec = xr_new<texture_spec>( );
			desc.m_spec->m_material = tp.material + tp.material_weight;

			if (tp.bump_mode == STextureParams::tbmUse)
			{
				desc.m_spec->m_bump_name = tp.bump_name;
			}
		}
	}
}

void CTextureDescrManager::Load( )
{
	CTimer TT;
	TT.Start( );

	LoadLTX( );
	LoadTHM( );

	Msg("load time=%d ms", TT.GetElapsed_ms( ));
}

void CTextureDescrManager::UnLoad( )
{
	map_TD::iterator I = m_texture_details.begin( );
	map_TD::iterator E = m_texture_details.end( );
	for (; I != E; ++I)
	{
		xr_delete(I->second.m_assoc);
		xr_delete(I->second.m_spec);
	}

	m_texture_details.clear( );
}

shared_str CTextureDescrManager::GetBumpName(const shared_str& tex_name) const
{
	map_TD::const_iterator I = m_texture_details.find(tex_name);
	if (I != m_texture_details.end( ))
	{
		if (I->second.m_spec)
		{
			return I->second.m_spec->m_bump_name;
		}
	}

	return "";
}

f32 CTextureDescrManager::GetMaterial(const shared_str& tex_name) const
{
	map_TD::const_iterator I = m_texture_details.find(tex_name);
	if (I != m_texture_details.end( ))
	{
		if (I->second.m_spec)
		{
			return I->second.m_spec->m_material;
		}
	}

	return 1.0f;
}

void CTextureDescrManager::GetTextureUsage(const shared_str& tex_name, BOOL& bDiffuse, BOOL& bBump) const
{
	map_TD::const_iterator I = m_texture_details.find(tex_name);
	if (I != m_texture_details.end( ))
	{
		if (I->second.m_assoc)
		{
			u8 usage = I->second.m_assoc->usage;
			bDiffuse = !!(usage & (1 << 0));
			bBump = !!(usage & (1 << 1));
		}
	}
}

BOOL CTextureDescrManager::GetDetailTexture(const shared_str& tex_name, pcstr& res, R_constant_setup*& CS) const
{
	map_TD::const_iterator I = m_texture_details.find(tex_name);
	if (I != m_texture_details.end( ))
	{
		if (I->second.m_assoc)
		{
			texture_assoc* TA = I->second.m_assoc;
			res = TA->detail_name.c_str( );
			CS = TA->cs;
			return TRUE;
		}
	}

	return FALSE;
}
