// file:		UITextureMaster.h
// description:	holds info about shared textures. able to initialize external controls
//				through IUITextureControl interface
// created:		11.05.2005
// author:		Serge Vynnychenko
// mail:		narrator@gsc-game.kiev.ua
//
// copyright 2005 GSC Game World


#include "stdafx.h"
#include "UITextureMaster.h"
#include "uiabstract.h"
#include "xrUIXmlParser.h"

xr_map<shared_str, TEX_INFO>	CUITextureMaster::m_textures;
#ifdef DEBUG
u32									CUITextureMaster::m_time = 0;
#endif

void CUITextureMaster::WriteLog( )
{
#ifdef DEBUG
	Msg("UI texture manager work time is %d ms", m_time);
#endif
}
void CUITextureMaster::ParseShTexInfo(pcstr xml_file)
{
	CUIXml uiXml;
	uiXml.Init(CONFIG_PATH, UI_PATH, xml_file);
	shared_str file = uiXml.Read("file_name", 0, "");

	s32 num = uiXml.GetNodesNum("", 0, "texture");
	// regions regs;
	for (int i = 0; i < num; i++)
	{
		TEX_INFO info;

		info.file = file;

		info.rect.x1 = uiXml.ReadAttribFlt("texture", i, "x");
		info.rect.x2 = uiXml.ReadAttribFlt("texture", i, "width") + info.rect.x1;
		info.rect.y1 = uiXml.ReadAttribFlt("texture", i, "y");
		info.rect.y2 = uiXml.ReadAttribFlt("texture", i, "height") + info.rect.y1;
		shared_str id = uiXml.ReadAttrib("texture", i, "id");

		m_textures.insert(mk_pair(id, info));
	}
}

bool CUITextureMaster::IsSh(pcstr texture_name)
{
	return strstr(texture_name, "\\") ? false : true;
}

void CUITextureMaster::InitTexture(pcstr texture_name, IUISimpleTextureControl* tc)
{
#ifdef DEBUG
	CTimer T;
	T.Start( );
#endif

	xr_map<shared_str, TEX_INFO>::iterator	it;

	it = m_textures.find(texture_name);

	if (it != m_textures.end( ))
	{
		tc->CreateShader(*((*it).second.file));
		tc->SetOriginalRectEx((*it).second.rect);
#ifdef DEBUG
		m_time += T.GetElapsed_ms( );
#endif
		return;
	}
	tc->CreateShader(texture_name);
#ifdef DEBUG
	m_time += T.GetElapsed_ms( );
#endif
}

void CUITextureMaster::InitTexture(pcstr texture_name, pcstr shader_name, IUISimpleTextureControl* tc)
{
#ifdef DEBUG
	CTimer T;
	T.Start( );
#endif

	xr_map<shared_str, TEX_INFO>::iterator	it;

	it = m_textures.find(texture_name);

	if (it != m_textures.end( ))
	{
		tc->CreateShader(*((*it).second.file), shader_name);
		tc->SetOriginalRectEx((*it).second.rect);
#ifdef DEBUG
		m_time += T.GetElapsed_ms( );
#endif
		return;
	}
	tc->CreateShader(texture_name, shader_name);
#ifdef DEBUG
	m_time += T.GetElapsed_ms( );
#endif
}

f32 CUITextureMaster::GetTextureHeight(pcstr texture_name)
{
	xr_map<shared_str, TEX_INFO>::iterator	it;
	it = m_textures.find(texture_name);

	if (it != m_textures.end( ))
		return (*it).second.rect.height( );
	R_ASSERT3(false, "CUITextureMaster::GetTextureHeight Can't find texture", texture_name);
	return 0;
}

Frect CUITextureMaster::GetTextureRect(pcstr texture_name)
{
	xr_map<shared_str, TEX_INFO>::iterator	it;
	it = m_textures.find(texture_name);
	if (it != m_textures.end( ))
		return (*it).second.rect;

	R_ASSERT3(false, "CUITextureMaster::GetTextureHeight Can't find texture", texture_name);
	return Frect( );
}

f32 CUITextureMaster::GetTextureWidth(pcstr texture_name)
{
	xr_map<shared_str, TEX_INFO>::iterator	it;
	it = m_textures.find(texture_name);

	if (it != m_textures.end( ))
		return (*it).second.rect.width( );
	R_ASSERT3(false, "CUITextureMaster::GetTextureHeight Can't find texture", texture_name);
	return 0;
}

pcstr CUITextureMaster::GetTextureFileName(pcstr texture_name)
{
	xr_map<shared_str, TEX_INFO>::iterator	it;
	it = m_textures.find(texture_name);

	if (it != m_textures.end( ))
		return *((*it).second.file);
	R_ASSERT3(false, "CUITextureMaster::GetTextureFileName Can't find texture", texture_name);
	return 0;
}

TEX_INFO CUITextureMaster::FindItem(pcstr texture_name, pcstr def_texture_name)
{
	xr_map<shared_str, TEX_INFO>::iterator	it;
	it = m_textures.find(texture_name);

	if (it != m_textures.end( ))
		return (it->second);
	else
	{
		R_ASSERT2(m_textures.find(def_texture_name) != m_textures.end( ), texture_name);
		return FindItem(def_texture_name, NULL);
	}
}

void CUITextureMaster::GetTextureShader(pcstr texture_name, ref_shader& sh)
{
	xr_map<shared_str, TEX_INFO>::iterator	it;
	it = m_textures.find(texture_name);

	R_ASSERT3(it != m_textures.end( ), "can't find texture", texture_name);

	sh.create("hud\\default", *((*it).second.file));
}