#pragma once

//#include "ETextureParams.h"

class CTextureDescrManager
{
	struct texture_assoc
	{
		shared_str			detail_name;
		R_constant_setup* cs;
		u8				usage;
		texture_assoc( ) : cs(NULL), usage(0)
		{ }
	};
	struct texture_spec
	{
		shared_str			m_bump_name;
		f32				m_material;
	};
	struct texture_desc
	{
		texture_assoc* m_assoc;
		texture_spec* m_spec;
		texture_desc( ) : m_assoc(NULL), m_spec(NULL)
		{ }
	};
	DEFINE_MAP(shared_str, texture_desc, map_TD, map_TDIt);

	map_TD									m_texture_details;

	void		LoadLTX( );
	void		LoadTHM( );

public:
	void		Load( );
	void		UnLoad( );

public:
	shared_str	GetBumpName(const shared_str& tex_name) const;
	f32			GetMaterial(const shared_str& tex_name) const;
	void		GetTextureUsage(const shared_str& tex_name, BOOL& bDiffuse, BOOL& bBump) const;
	BOOL		GetDetailTexture(const shared_str& tex_name, pcstr& res, R_constant_setup*& CS) const;
};
