#pragma once

inline	bool CCustomMonster::angle_lerp_bounds(float &a, float b, float c, float d)
{
	if (c*d >= angle_difference(a,b)) {
		a = b;
		return(true);
	}
	
	angle_lerp(a,b,c,d);

	return(false);
};

inline void CCustomMonster::vfNormalizeSafe(Fvector3& Vector)
{
	float fMagnitude = Vector.magnitude(); 
	if (fMagnitude > EPS_L) {
		Vector.x /= fMagnitude;
		Vector.y /= fMagnitude;
		Vector.z /= fMagnitude;
	}
	else {
		Vector.x = 1.f;
		Vector.y = 0.f;
		Vector.z = 0.f;
	}
}

__forceinline	bool left_angle(float y1, float y2)
{
	return			(_sin(y1)*_cos(y2) - _sin(y2)*_cos(y1) <= 0.f);
}

inline	CMemoryManager &CCustomMonster::memory		() const
{
	VERIFY			(m_memory_manager);
	return			(*m_memory_manager);
}

inline	CMovementManager &CCustomMonster::movement	() const
{
	VERIFY			(m_movement_manager);
	return			(*m_movement_manager);
}

inline	CSoundPlayer &CCustomMonster::sound			() const
{
	VERIFY			(m_sound_player);
	return			(*m_sound_player);
}

inline	CSound_UserDataVisitor *CCustomMonster::sound_user_data_visitor	() const
{
	VERIFY			(m_sound_user_data_visitor);
	return			(m_sound_user_data_visitor);
}

inline	float CCustomMonster::panic_threshold				() const
{
	return			(m_panic_threshold);
}

inline	float CCustomMonster::client_update_fdelta			() const
{
	return			((float)m_client_update_delta/1000.f);
}

inline	const u32 &CCustomMonster::client_update_delta		() const
{
	return			(m_client_update_delta);
}

inline	const u32 &CCustomMonster::last_client_update_time	() const
{
	return			(m_last_client_update_time);
}

inline	const u32 &CCustomMonster::critical_wound_type	() const
{
	return			(m_critical_wound_type);
}

inline bool CCustomMonster::critically_wounded			()
{
	return			(m_critical_wound_type != u32(-1));
}

inline void CCustomMonster::critical_wounded_state_stop	() 
{
	m_critical_wound_type = u32(-1);
}

inline		void CCustomMonster::invulnerable			(const bool &invulnerable)
{
	m_invulnerable	= invulnerable;
}

inline		bool CCustomMonster::invulnerable			() const
{
	return			(m_invulnerable);
}
