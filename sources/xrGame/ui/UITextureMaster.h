// ========================================== SHB_Engine ==========================================
// Projekt		: Game
// Module		: UITextureMaster.h
// Author		: Anahoret
// Description	: содержит общую информацию о текстурах. Возможность инициализации внешних текстур
//				  через интерфейс IUISimpleTextureControl
// ========================================= SovaHiBlack© =========================================

#pragma once
#ifndef GAME_UITEXTUREMASTER_H_INCLUDED
#define GAME_UITEXTUREMASTER_H_INCLUDED

class IUISimpleTextureControl;

struct STextureInfo
{
	shared_str										file;
	fRect											rect;
	pcstr					get_file_name			( )
	{
		return *file;
	}
	fRect					get_rect				( )
	{
		return rect;
	}
};

class CUITextureMaster
{
public:
	static void				ParseShTexInfo			(pcstr xml_file);

	static void				InitTexture				(pcstr texture_name, IUISimpleTextureControl* tc);
	static void				InitTexture				(pcstr texture_name, pcstr shader_name, IUISimpleTextureControl* tc);
	static f32				GetTextureHeight		(pcstr texture_name);
	static f32				GetTextureWidth			(pcstr texture_name);
	static fRect			GetTextureRect			(pcstr texture_name);
	static pcstr			GetTextureFileName		(pcstr texture_name);
	static void				GetTextureShader		(pcstr texture_name, ref_shader& sh);
	static STextureInfo		FindItem				(pcstr texture_name, pcstr def_texture_name);
	static void				WriteLog				( );

protected:
	IC static bool			IsSh					(pcstr texture_name);

	static xr_map<shared_str, STextureInfo>			m_textures;

#ifdef DEBUG
	static u32										m_time;
#endif // def DEBUG

};

#endif // ndef GAME_UITEXTUREMASTER_H_INCLUDED