#include "stdafx.h"

#include "PHSoundPlayer.h"
#include "PHShellHolder.h"

CPHSoundPlayer::CPHSoundPlayer(CPHShellHolder* obj)
{
	m_object = obj;
}

CPHSoundPlayer::~CPHSoundPlayer( )
{
	m_sound.stop( );
	m_object = NULL;
}

void CPHSoundPlayer::Play(SGameMtlPair* mtl_pair, const Fvector3& pos)
{
	if (!m_sound._feedback( ))
	{
		Fvector3 vel;
		m_object->PHGetLinearVell(vel);
		if (vel.square_magnitude( ) > 0.01f)
		{
			CLONE_MTL_SOUND(m_sound, mtl_pair, CollideSounds);
			m_sound.play_at_pos(smart_cast<CPHShellHolder*>(m_object), pos);
		}
	}
}
