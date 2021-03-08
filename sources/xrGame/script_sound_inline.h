////////////////////////////////////////////////////////////////////////////
//	Module 		: script_sound_inline.h
//	Created 	: 06.02.2004
//  Modified 	: 06.02.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay Script sound class inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

inline	u32	CScriptSound::Length				()
{
	VERIFY					(m_sound._handle());
	return					(m_sound._handle()->length_ms());
}

inline	void CScriptSound::Play					(CScriptGameObject *object)
{
	Play					(object,0.f,0);
}

inline	void CScriptSound::Play					(CScriptGameObject *object, float delay)
{
	Play					(object,delay,0);
}
		
inline	void CScriptSound::PlayAtPos			(CScriptGameObject *object, const Fvector &position)
{
	PlayAtPos				(object,position,0.f,0);
}

inline	void CScriptSound::PlayAtPos			(CScriptGameObject *object, const Fvector &position, float delay)
{
	PlayAtPos				(object,position,delay,0);
}

inline	void CScriptSound::SetMinDistance		(const float fMinDistance)
{
	VERIFY				(m_sound._handle());
	m_sound.set_range(fMinDistance,GetMaxDistance());
}

inline	void CScriptSound::SetMaxDistance		(const float fMaxDistance)
{
	VERIFY				(m_sound._handle());
	m_sound.set_range(GetMinDistance(),fMaxDistance);
}

inline	const float	CScriptSound::GetFrequency	() const
{
	VERIFY				(m_sound._handle());
	return				(m_sound.get_params()->freq);
}

inline	const float CScriptSound::GetMinDistance() const
{
	VERIFY				(m_sound._handle());
	return				(m_sound.get_params()->min_distance);
}

inline	const float CScriptSound::GetMaxDistance() const
{
	VERIFY				(m_sound._handle());
	return				(m_sound.get_params()->max_distance);
}

inline	const float	CScriptSound::GetVolume		() const
{
	VERIFY				(m_sound._handle());
	return				(m_sound.get_params()->volume);
}

inline	bool CScriptSound::IsPlaying			() const
{
	VERIFY				(m_sound._handle());
	return				(!!m_sound._feedback());
}

inline	void CScriptSound::Stop					()
{
	VERIFY				(m_sound._handle());
	m_sound.stop		();
}

inline	void CScriptSound::StopDeffered			()
{
	VERIFY				(m_sound._handle());
	m_sound.stop_deffered();
}

inline	void CScriptSound::SetPosition			(const Fvector &position)
{
	VERIFY				(m_sound._handle());
	m_sound.set_position(position);
}

inline	void CScriptSound::SetFrequency			(float frequency)
{
	VERIFY				(m_sound._handle());
	m_sound.set_frequency(frequency);
}

inline	void CScriptSound::SetVolume			(float volume)
{
	VERIFY				(m_sound._handle());
	m_sound.set_volume	(volume);
}

inline	const CSound_params *CScriptSound::GetParams	()
{
	VERIFY				(m_sound._handle());
	return				(m_sound.get_params());
}

inline	void CScriptSound::SetParams			(CSound_params *sound_params)
{
	VERIFY				(m_sound._handle());
	m_sound.set_params	(sound_params);
}
