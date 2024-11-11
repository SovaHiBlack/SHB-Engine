// TextureManager.cpp: implementation of the CResourceManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#pragma warning(disable:4995)
#include <d3dx9.h>
#pragma warning(default:4995)

#include "ResourceManager.h"
//#include "tss.h"
#include "blenders\blender.h"
#include "blenders\blender_recorder.h"

void fix_texture_name(pstr fn)
{
	pstr _ext = strext(fn);
	if (_ext &&
		(0 == stricmp(_ext, ".tga") ||
		 0 == stricmp(_ext, ".dds") ||
		 0 == stricmp(_ext, ".bmp") ||
		 0 == stricmp(_ext, ".ogm")))
		*_ext = 0;
}
//--------------------------------------------------------------------------------------------------------------
template <class T>
BOOL	reclaim(xr_vector<T*>& vec, const T* ptr)
{
	xr_vector<T*>::iterator it = vec.begin( );
	xr_vector<T*>::iterator end = vec.end( );
	for (; it != end; it++)
	{
		if (*it == ptr)
		{
			vec.erase(it); return TRUE;
		}
	}

	return FALSE;
}

//--------------------------------------------------------------------------------------------------------------
IBlender* CResourceManager::_GetBlender(pcstr Name)
{
	R_ASSERT(Name && Name[0]);

	pstr N = pstr(Name);
	map_Blender::iterator I = m_blenders.find(N);

	if (I == m_blenders.end( ))
	{
		Debug.fatal(DEBUG_INFO, "Shader '%s' not found in library.", Name);
		return 0;
	}
	else
	{
		return I->second;
	}
}

IBlender* CResourceManager::_FindBlender(pcstr Name)
{
	if (!(Name && Name[0]))
	{
		return 0;
	}

	pstr N = pstr(Name);
	map_Blender::iterator I = m_blenders.find(N);
	if (I == m_blenders.end( ))
	{
		return 0;
	}
	else
	{
		return I->second;
	}
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
void	CResourceManager::_ParseList(sh_list& dest, pcstr names)
{
	if (0 == names)
	{
		names = "$null";
	}

	ZeroMemory(&dest, sizeof(dest));
	pstr P = pstr(names);
	svector<char, 128>	N;

	while (*P)
	{
		if (*P == ',')
		{
			// flush
			N.push_back(0);
			strlwr(N.begin( ));

			fix_texture_name(N.begin( ));
			dest.push_back(N.begin( ));
			N.clear( );
		}
		else
		{
			N.push_back(*P);
		}

		P++;
	}

	if (N.size( ))
	{
		// flush
		N.push_back(0);
		strlwr(N.begin( ));

		fix_texture_name(N.begin( ));
		dest.push_back(N.begin( ));
	}
}

ShaderElement* CResourceManager::_CreateElement(ShaderElement& S)
{
	if (S.passes.empty( ))
	{
		return	0;
	}

	// Search equal in shaders array
	for (u32 it = 0; it < v_elements.size( ); it++)
	{
		if (S.equal(*(v_elements[it])))
		{
			return v_elements[it];
		}
	}

	// Create _new_ entry
	ShaderElement* N = xr_new<ShaderElement>(S);
	N->dwFlags |= xr_resource_flagged::RF_REGISTERED;
	v_elements.push_back(N);
	return N;
}

void CResourceManager::_DeleteElement(const ShaderElement* S)
{
	if (0 == (S->dwFlags & xr_resource_flagged::RF_REGISTERED))
	{
		return;
	}

	if (reclaim(v_elements, S))
	{
		return;
	}

	Msg("! ERROR: Failed to find compiled 'shader-element'");
}

Shader* CResourceManager::_cpp_Create(IBlender* B, pcstr s_shader, pcstr s_textures, pcstr s_constants, pcstr s_matrices)
{
	CBlender_Compile	C;
	Shader				S;

	// Access to template
	C.BT = B;
	C.bEditor = FALSE;
	C.bDetail = FALSE;

	// Parse names
	_ParseList(C.L_textures, s_textures);
	_ParseList(C.L_constants, s_constants);
	_ParseList(C.L_matrices, s_matrices);

	// Compile element	(LOD0 - HQ)
	{
		C.iElement = 0;
		C.bDetail = m_textures_description.GetDetailTexture(C.L_textures[0], C.detail_texture, C.detail_scaler);
		ShaderElement		E;
		C._cpp_Compile(&E);
		S.E[0] = _CreateElement(E);
	}

	// Compile element	(LOD1)
	{
		C.iElement = 1;
		C.bDetail = m_textures_description.GetDetailTexture(C.L_textures[0], C.detail_texture, C.detail_scaler);
		ShaderElement		E;
		C._cpp_Compile(&E);
		S.E[1] = _CreateElement(E);
	}

	// Compile element
	{
		C.iElement = 2;
		C.bDetail = FALSE;
		ShaderElement		E;
		C._cpp_Compile(&E);
		S.E[2] = _CreateElement(E);
	}

	// Compile element
	{
		C.iElement = 3;
		C.bDetail = FALSE;
		ShaderElement		E;
		C._cpp_Compile(&E);
		S.E[3] = _CreateElement(E);
	}

	// Compile element
	{
		C.iElement = 4;
		C.bDetail = TRUE;	//.$$$ HACK :)
		ShaderElement		E;
		C._cpp_Compile(&E);
		S.E[4] = _CreateElement(E);
	}

	// Compile element
	{
		C.iElement = 5;
		C.bDetail = FALSE;
		ShaderElement		E;
		C._cpp_Compile(&E);
		S.E[5] = _CreateElement(E);
	}

	// Search equal in shaders array
	for (u32 it = 0; it < v_shaders.size( ); it++)
	{
		if (S.equal(v_shaders[it]))
		{
			return v_shaders[it];
		}
	}

	// Create _new_ entry
	Shader* N = xr_new<Shader>(S);
	N->dwFlags |= xr_resource_flagged::RF_REGISTERED;
	v_shaders.push_back(N);
	return N;
}

Shader* CResourceManager::_cpp_Create(pcstr s_shader, pcstr s_textures, pcstr s_constants, pcstr s_matrices)
{
	return	_cpp_Create(_GetBlender(s_shader ? s_shader : "null"), s_shader, s_textures, s_constants, s_matrices);
}

Shader* CResourceManager::Create(IBlender* B, pcstr s_shader, pcstr s_textures, pcstr s_constants, pcstr s_matrices)
{
	return	_cpp_Create(B, s_shader, s_textures, s_constants, s_matrices);
}

Shader* CResourceManager::Create(pcstr s_shader, pcstr s_textures, pcstr s_constants, pcstr s_matrices)
{
	if (_lua_HasShader(s_shader))
	{
		return	_lua_Create(s_shader, s_textures);
	}
	else
	{
		return	_cpp_Create(s_shader, s_textures, s_constants, s_matrices);
	}
}

void CResourceManager::Delete(const Shader* S)
{
	if (0 == (S->dwFlags & xr_resource_flagged::RF_REGISTERED))
	{
		return;
	}

	if (reclaim(v_shaders, S))
	{
		return;
	}

	Msg("! ERROR: Failed to find complete shader");
}

void	CResourceManager::DeferredUpload( )
{
	if (!Device.b_is_Ready)
	{
		return;
	}

	for (map_TextureIt t = m_textures.begin( ); t != m_textures.end( ); t++)
	{
		t->second->Load( );
	}
}

void	CResourceManager::_GetMemoryUsage(u32& m_base, u32& c_base, u32& m_lmaps, u32& c_lmaps)
{
	m_base = c_base = m_lmaps = c_lmaps = 0;

	map_Texture::iterator I = m_textures.begin( );
	map_Texture::iterator E = m_textures.end( );
	for (; I != E; I++)
	{
		u32 m = I->second->flags.MemoryUsage;
		if (strstr(I->first, "lmap"))
		{
			c_lmaps++;
			m_lmaps += m;
		}
		else
		{
			c_base++;
			m_base += m;
		}
	}
}
void	CResourceManager::_DumpMemoryUsage( )
{
	xr_multimap<u32, std::pair<u32, shared_str> >		mtex;

	// sort
	{
		map_Texture::iterator I = m_textures.begin( );
		map_Texture::iterator E = m_textures.end( );
		for (; I != E; I++)
		{
			u32			m = I->second->flags.MemoryUsage;
			shared_str	n = I->second->cName;
			mtex.insert(mk_pair(m, mk_pair(I->second->dwReference, n)));
		}
	}

	// dump
	{
		xr_multimap<u32, std::pair<u32, shared_str> >::iterator I = mtex.begin( );
		xr_multimap<u32, std::pair<u32, shared_str> >::iterator E = mtex.end( );
		for (; I != E; I++)
		{
			Msg("* %4.1f : [%4d] %s", f32(I->first) / 1024.f, I->second.first, I->second.second.c_str( ));
		}
	}
}

void	CResourceManager::Evict( )
{
	CHK_DX(HW.pDevice->EvictManagedResources( ));
}
