//////////////////////////////////////////////////////////////////////
// HudSound.h:		структура для работы со звуками применяемыми в 
//					HUD-объектах (обычные звуки, но с доп. параметрами)
//////////////////////////////////////////////////////////////////////

#pragma once

struct HUD_SOUND
{
	HUD_SOUND( )
	{
		m_activeSnd = NULL;
	}
	~HUD_SOUND( )
	{
		m_activeSnd = NULL;
	}

////////////////////////////////////
// работа со звуками
/////////////////////////////////////
	static void		LoadSound(pcstr section, pcstr line,
							  ref_sound& hud_snd,
							  s32 type = sg_SourceType,
							  f32* volume = NULL,
							  f32* delay = NULL);

	static void		LoadSound(pcstr section, pcstr line,
							  HUD_SOUND& hud_snd, s32 type = sg_SourceType);

	static void		DestroySound(HUD_SOUND& hud_snd);

	static void		PlaySound(HUD_SOUND& snd,
							  const fVector3& position,
							  const CObject* parent,
							  bool hud_mode,
							  bool looped = false);

	static void		StopSound(HUD_SOUND& snd);

	ICF BOOL		playing( )
	{
		if (m_activeSnd)
		{
			return	m_activeSnd->snd._feedback( ) ? TRUE : FALSE;
		}
		else
		{
			return	FALSE;
		}
	}

	ICF void		set_position(const fVector3& pos)
	{
		if (m_activeSnd)
		{
			if (m_activeSnd->snd._feedback( ) && !m_activeSnd->snd._feedback( )->is_2D( ))
			{
				m_activeSnd->snd.set_position(pos);
			}
			else
			{
				m_activeSnd = NULL;
			}
		}
	}

	struct SSnd
	{
		ref_sound	snd;
		f32		delay;		//задержка перед проигрыванием
		f32		volume;		//громкость
	};
	SSnd* m_activeSnd;
	xr_vector<SSnd> sounds;
};
