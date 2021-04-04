// file:		UITextureMaster.h
// description:	holds info about shared textures. able to initialize external controls through IUITextureControl interface

#include "stdafx.h"

#include "UITextureMaster.h"//
#include "uiabstract.h"
#include "xrUIXmlParser.h"//

xr_map<shared_str, TEX_INFO>	CUITextureMaster::m_textures;

#ifdef DEBUG
u32 CUITextureMaster::m_time = 0;
#endif // def DEBUG

void CUITextureMaster::WriteLog( )
{

#ifdef DEBUG
	Msg("UI texture manager work time is %d ms", m_time);
#endif // def DEBUG

}

void CUITextureMaster::ParseShTexInfo(const char* xml_file)
{
	CUIXml xml;
	xml.Init("$game_config$", "ui", xml_file);
	shared_str file = xml.Read("file_name", 0, "");

	int num = xml.GetNodesNum("", 0, "texture");

	for (int i = 0; i < num; i++)
	{
		TEX_INFO info;

		info.file = file;

		info.rect.x1 = xml.ReadAttribFlt("texture", i, "x");
		info.rect.x2 = xml.ReadAttribFlt("texture", i, "width") + info.rect.x1;
		info.rect.y1 = xml.ReadAttribFlt("texture", i, "y");
		info.rect.y2 = xml.ReadAttribFlt("texture", i, "height") + info.rect.y1;
		shared_str id = xml.ReadAttrib("texture", i, "id");

		m_textures.insert(mk_pair(id, info));
	}
}

bool CUITextureMaster::IsSh(const char* texture_name)
{
	return strstr(texture_name, "\\") ? false : true;
}

void CUITextureMaster::InitTexture(const char* texture_name, IUISimpleTextureControl* tc)
{

#ifdef DEBUG
	CTimer T;
	T.Start( );
#endif // def DEBUG

	xr_map<shared_str, TEX_INFO>::iterator	it;
	it = m_textures.find(texture_name);
	if (it != m_textures.end( ))
	{
		tc->CreateShader(*((*it).second.file));
		tc->SetOriginalRectEx((*it).second.rect);

#ifdef DEBUG
		m_time += T.GetElapsed_ms( );
#endif // def DEBUG

		return;
	}

	tc->CreateShader(texture_name);

#ifdef DEBUG
	m_time += T.GetElapsed_ms( );
#endif // def DEBUG

}

void CUITextureMaster::InitTexture(const char* texture_name, const char* shader_name, IUISimpleTextureControl* tc)
{

#ifdef DEBUG
	CTimer T;
	T.Start( );
#endif // def DEBUG

	xr_map<shared_str, TEX_INFO>::iterator	it;
	it = m_textures.find(texture_name);
	if (it != m_textures.end( ))
	{
		tc->CreateShader(*((*it).second.file), shader_name);
		tc->SetOriginalRectEx((*it).second.rect);

#ifdef DEBUG
		m_time += T.GetElapsed_ms( );
#endif // def DEBUG

		return;
	}

	tc->CreateShader(texture_name, shader_name);

#ifdef DEBUG
	m_time += T.GetElapsed_ms( );
#endif // def DEBUG

}

float CUITextureMaster::GetTextureHeight(const char* texture_name)
{
	xr_map<shared_str, TEX_INFO>::iterator	it;
	it = m_textures.find(texture_name);

	if (it != m_textures.end( ))
	{
		return (*it).second.rect.height( );
	}

	R_ASSERT3(false, "CUITextureMaster::GetTextureHeight Can't find texture", texture_name);
	return 0.0f;
}

Frect CUITextureMaster::GetTextureRect(const char* texture_name)
{
	xr_map<shared_str, TEX_INFO>::iterator	it;
	it = m_textures.find(texture_name);
	if (it != m_textures.end( ))
	{
		return (*it).second.rect;
	}

	R_ASSERT3(false, "CUITextureMaster::GetTextureHeight Can't find texture", texture_name);
	return Frect( );
}

float CUITextureMaster::GetTextureWidth(const char* texture_name)
{
	xr_map<shared_str, TEX_INFO>::iterator	it;
	it = m_textures.find(texture_name);
	if (it != m_textures.end( ))
	{
		return (*it).second.rect.width( );
	}

	R_ASSERT3(false, "CUITextureMaster::GetTextureHeight Can't find texture", texture_name);
	return 0.0f;
}

const char* CUITextureMaster::GetTextureFileName(const char* texture_name)
{
	xr_map<shared_str, TEX_INFO>::iterator	it;
	it = m_textures.find(texture_name);
	if (it != m_textures.end( ))
	{
		return *((*it).second.file);
	}

	R_ASSERT3(false, "CUITextureMaster::GetTextureFileName Can't find texture", texture_name);
	return 0;
}

TEX_INFO CUITextureMaster::FindItem(const char* texture_name, const char* def_texture_name)
{
	xr_map<shared_str, TEX_INFO>::iterator	it;
	it = m_textures.find(texture_name);
	if (it != m_textures.end( ))
	{
		return (it->second);
	}
	else
	{
		R_ASSERT2(m_textures.find(def_texture_name) != m_textures.end( ), texture_name);
		return FindItem(def_texture_name, NULL);
	}
}

void CUITextureMaster::GetTextureShader(const char* texture_name, ref_shader& sh)
{
	xr_map<shared_str, TEX_INFO>::iterator	it;
	it = m_textures.find(texture_name);

	R_ASSERT3(it != m_textures.end( ), "can't find texture", texture_name);

	sh.create("hud\\default", *((*it).second.file));
}
