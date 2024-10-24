// file:		UITextureMaster.h
// description:	holds info about shared textures. able to initialize external
//				through IUITextureControl interface
// created:		11.05.2005
// author:		Serge Vynnychenko
// mail:		narrator@gsc-game.kiev.ua
//
// copyright 2005 GSC Game World

#pragma once

class IUISimpleTextureControl;

struct TEX_INFO{
	shared_str	file;
	fRect		rect;
	pcstr		get_file_name	()	{return *file;}
	fRect		get_rect		()	{return rect;}
};

class CUITextureMaster{
public:

	static void ParseShTexInfo			(pcstr xml_file);

	static void		InitTexture			(pcstr texture_name,		IUISimpleTextureControl* tc);
	static void		InitTexture			(pcstr texture_name, pcstr shader_name, IUISimpleTextureControl* tc);
	static f32	GetTextureHeight	(pcstr texture_name);
	static f32	GetTextureWidth		(pcstr texture_name);
	static fRect	GetTextureRect		(pcstr texture_name);
	static pcstr	GetTextureFileName	(pcstr texture_name);
	static void		GetTextureShader	(pcstr texture_name, ref_shader& sh);
	static TEX_INFO	FindItem			(pcstr texture_name, pcstr def_texture_name);
	static void WriteLog();

protected:
	IC	static bool IsSh					(pcstr texture_name);



//	typedef xr_string region_name;
//	typedef xr_string shader_name;
//	typedef xr_map<region_name, fRect>				regions;
//	typedef xr_map<region_name, fRect>::iterator	regions_it;
//	typedef xr_map<shader_name, regions>			shared_textures;
//	typedef xr_map<shader_name, regions>::iterator	shared_textures_it;

	static xr_map<shared_str, TEX_INFO>					m_textures;

//	static	shared_textures		m_shTex;
#ifdef DEBUG
	static u32		m_time;
#endif 
};