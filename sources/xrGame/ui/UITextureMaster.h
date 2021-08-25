// file:		UITextureMaster.h
// description:	holds info about shared textures. able to initialize external through IUITextureControl interface

#pragma once

class IUISimpleTextureControl;

struct TEX_INFO
{
	CSharedString									file;
	Frect										rect;
	const char*			get_file_name			( )
	{
		return *file;
	}
	Frect				get_rect				( )
	{
		return rect;
	}
};

class CUITextureMaster
{
public:
	static void			ParseShTexInfo			(const char* xml_file);

	static void			InitTexture				(const char* texture_name, IUISimpleTextureControl* tc);
	static void			InitTexture				(const char* texture_name, const char* shader_name, IUISimpleTextureControl* tc);
	static float		GetTextureHeight		(const char* texture_name);
	static float		GetTextureWidth			(const char* texture_name);
	static Frect		GetTextureRect			(const char* texture_name);
	static const char*	GetTextureFileName		(const char* texture_name);
	static void			GetTextureShader		(const char* texture_name, ref_shader& sh);
	static TEX_INFO		FindItem				(const char* texture_name, const char* def_texture_name);
	static void			WriteLog				( );

protected:
	inline static bool	IsSh					(const char* texture_name);

	static xr_map<CSharedString, TEX_INFO>		m_textures;

#ifdef DEBUG
	static u32									m_time;
#endif // def DEBUG

};
