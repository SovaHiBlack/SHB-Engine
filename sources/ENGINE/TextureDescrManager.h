#pragma once

#include "ETextureParams.h"

class CTextureDescrMngr
{
	struct texture_assoc
	{
		CSharedString			detail_name;
		R_constant_setup*	        cs;
		U8				usage;
				texture_assoc                   ():cs(NULL),usage(0){}
	};
	struct texture_spec
	{
		CSharedString			m_bump_name;
		float				m_material;
	};
	struct texture_desc{
		texture_assoc*		m_assoc;
		texture_spec*		m_spec;
		texture_desc            ():m_assoc(NULL),m_spec(NULL){}
	};
//	DEFINE_MAP(CSharedString, texture_desc,	map_TD,	map_TDIt);
	using map_TD = xr_map<CSharedString, texture_desc>;
	using map_TDIt = map_TD::iterator;
	map_TD									m_texture_details;

	void		LoadLTX		();
	void		LoadTHM		();

public:
	void		Load		();
	void		UnLoad		();
public:
	CSharedString	GetBumpName		(const CSharedString& tex_name) const;
	float		GetMaterial		(const CSharedString& tex_name) const;
	void		GetTextureUsage	(const CSharedString& tex_name, BOOL& bDiffuse, BOOL& bBump) const;
	BOOL		GetDetailTexture(const CSharedString& tex_name, const char*& res, R_constant_setup* &CS) const;
};
