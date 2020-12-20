#pragma once

#include "GameMtlLib.h"

class CPHShellHolder;

class CPHSoundPlayer
{
	ref_sound					m_sound;
	CPHShellHolder*		m_object;

public:
	void						Play				(SGameMtlPair* mtl_pair, const Fvector& pos);
								CPHSoundPlayer		(CPHShellHolder* m_object);
	virtual						~CPHSoundPlayer		( );
};