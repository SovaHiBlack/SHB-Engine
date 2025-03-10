#pragma once

IC	bool CCustomMonster::angle_lerp_bounds(f32& a, f32 b, f32 c, f32 d)
{
	if (c*d >= angle_difference(a,b)) {
		a = b;
		return(true);
	}
	
	angle_lerp(a,b,c,d);

	return(false);
};

IC void CCustomMonster::vfNormalizeSafe(fVector3& Vector)
{
	f32 fMagnitude = Vector.magnitude();
	if (fMagnitude > EPSILON_3) {
		Vector.x /= fMagnitude;
		Vector.y /= fMagnitude;
		Vector.z /= fMagnitude;
	}
	else {
		Vector.x = 1.0f;
		Vector.y = 0.0f;
		Vector.z = 0.0f;
	}
}

ICF	bool left_angle(f32 y1, f32 y2)
{
	return			(_sin(y1)*_cos(y2) - _sin(y2)*_cos(y1) <= 0.f);
}

IC	CMemoryManager &CCustomMonster::memory		() const
{
	VERIFY			(m_memory_manager);
	return			(*m_memory_manager);
}

IC	CMovementManager &CCustomMonster::movement	() const
{
	VERIFY			(m_movement_manager);
	return			(*m_movement_manager);
}

IC	CSoundPlayer &CCustomMonster::sound			() const
{
	VERIFY			(m_sound_player);
	return			(*m_sound_player);
}

IC	CSound_UserDataVisitor *CCustomMonster::sound_user_data_visitor	() const
{
	VERIFY			(m_sound_user_data_visitor);
	return			(m_sound_user_data_visitor);
}

IC	f32 CCustomMonster::panic_threshold				() const
{
	return			(m_panic_threshold);
}

IC	f32 CCustomMonster::client_update_fdelta			() const
{
	return			((f32)m_client_update_delta/1000.f);
}

IC	const u32 &CCustomMonster::client_update_delta		() const
{
	return			(m_client_update_delta);
}

IC	const u32 &CCustomMonster::last_client_update_time	() const
{
	return			(m_last_client_update_time);
}

IC	const u32 &CCustomMonster::critical_wound_type	() const
{
	return			(m_critical_wound_type);
}

IC bool CCustomMonster::critically_wounded			()
{
	return			(m_critical_wound_type != u32(-1));
}

IC void CCustomMonster::critical_wounded_state_stop	() 
{
	m_critical_wound_type = u32(-1);
}

IC		void CCustomMonster::invulnerable			(const bool &invulnerable)
{
	m_invulnerable	= invulnerable;
}

IC		bool CCustomMonster::invulnerable			() const
{
	return			(m_invulnerable);
}
