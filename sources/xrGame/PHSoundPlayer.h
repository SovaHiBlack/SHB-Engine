#pragma once

#include "GameMtlLib.h"

class CPhysicsShellHolder;

class CPHSoundPlayer
{
	ref_sound					m_sound;
	CPhysicsShellHolder*		m_object;

public:
	void						Play				(SGameMtlPair* mtl_pair, const fVector3& pos);
								CPHSoundPlayer		(CPhysicsShellHolder* m_object);
	virtual						~CPHSoundPlayer		( );
};