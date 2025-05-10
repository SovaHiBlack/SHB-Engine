//////////////////////////////////////////////////////////////////////
// HudSound.cpp:	структура для работы со звуками применяемыми в 
//					HUD-объектах (обычные звуки, но с доп. параметрами)
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "HudSound.h"

void HUD_SOUND::LoadSound(pcstr section, pcstr line, HUD_SOUND& hud_snd, s32 type)
{
	hud_snd.m_activeSnd = NULL;
	hud_snd.sounds.clear( );

	string256	sound_line;
	strcpy_s(sound_line, line);
	s32 k = 0;
	while (pSettings->line_exist(section, sound_line))
	{
		hud_snd.sounds.push_back(SSnd( ));
		SSnd& s = hud_snd.sounds.back( );

		LoadSound(section, sound_line, s.snd, type, &s.volume, &s.delay);
		sprintf_s(sound_line, "%s%d", line, ++k);
	}

	R_ASSERT3(!hud_snd.sounds.empty( ), "there is no sounds for:", section);
}

void  HUD_SOUND::LoadSound(pcstr section, pcstr line,
						   ref_sound& snd,
						   s32 type,
						   f32* volume,
						   f32* delay)
{
	pcstr str = pSettings->r_string(section, line);
	string256 buf_str;

	s32	count = _GetItemCount(str);
	R_ASSERT(count);

	_GetItem(str, 0, buf_str);
	snd.create(buf_str, st_Effect, type);

	if (volume != NULL)
	{
		*volume = 1.f;
		if (count > 1)
		{
			_GetItem(str, 1, buf_str);
			if (xr_strlen(buf_str) > 0)
			{
				*volume = (f32)atof(buf_str);
			}
		}
	}

	if (delay != NULL)
	{
		*delay = 0;
		if (count > 2)
		{
			_GetItem(str, 2, buf_str);
			if (xr_strlen(buf_str) > 0)
			{
				*delay = (f32)atof(buf_str);
			}
		}
	}
}

void HUD_SOUND::DestroySound(HUD_SOUND& hud_snd)
{
	xr_vector<SSnd>::iterator it = hud_snd.sounds.begin( );
	for (; it != hud_snd.sounds.end( ); ++it)
	{
		(*it).snd.destroy( );
	}
	hud_snd.sounds.clear( );

	hud_snd.m_activeSnd = NULL;
}

void HUD_SOUND::PlaySound(HUD_SOUND& hud_snd,
						  const fVector3& position,
						  const CObject* parent,
						  bool			b_hud_mode,
						  bool			looped)
{
	if (hud_snd.sounds.empty( ))
	{
		return;
	}

	hud_snd.m_activeSnd = NULL;
	StopSound(hud_snd);

	u32 flags = b_hud_mode ? sm_2D : 0;
	if (looped)
	{
		flags |= sm_Looped;
	}

	hud_snd.m_activeSnd = &hud_snd.sounds[Random.randI(hud_snd.sounds.size( ))];

	hud_snd.m_activeSnd->snd.play_at_pos(const_cast<CObject*>(parent),
										 flags & sm_2D ? fVector3( ).set(0.0f, 0.0f, 0.0f) : position,
										 flags,
										 /*0.f*/hud_snd.m_activeSnd->delay);
	hud_snd.m_activeSnd->snd.set_volume(hud_snd.m_activeSnd->volume);
}

void HUD_SOUND::StopSound(HUD_SOUND& hud_snd)
{
	xr_vector<SSnd>::iterator it = hud_snd.sounds.begin( );
	for (; it != hud_snd.sounds.end( ); ++it)
	{
//.		VERIFY2					((*it).snd._handle(),"Trying to stop non-existant or destroyed sound");
		(*it).snd.stop( );
	}

	hud_snd.m_activeSnd = NULL;
}
