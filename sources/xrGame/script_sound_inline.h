////////////////////////////////////////////////////////////////////////////
//	Module 		: script_sound_inline.h
//	Created 	: 06.02.2004
//  Modified 	: 06.02.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay Script sound class inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	u32	CScriptSound::Length				()
{
	VERIFY					(m_sound._handle());
	return					(m_sound._handle()->length_ms());
}

IC	void CScriptSound::Play					(CScriptGameObject *object)
{
	Play					(object,0.f,0);
}

IC	void CScriptSound::Play					(CScriptGameObject *object, f32 delay)
{
	Play					(object,delay,0);
}
		
IC	void CScriptSound::PlayAtPos			(CScriptGameObject *object, const fVector3& position)
{
	PlayAtPos				(object,position,0.f,0);
}

IC	void CScriptSound::PlayAtPos			(CScriptGameObject *object, const fVector3& position, f32 delay)
{
	PlayAtPos				(object,position,delay,0);
}

IC	void CScriptSound::SetMinDistance		(const f32 fMinDistance)
{
	VERIFY				(m_sound._handle());
	m_sound.set_range(fMinDistance,GetMaxDistance());
}

IC	void CScriptSound::SetMaxDistance		(const f32 fMaxDistance)
{
	VERIFY				(m_sound._handle());
	m_sound.set_range(GetMinDistance(),fMaxDistance);
}

IC	const f32	CScriptSound::GetFrequency	() const
{
	VERIFY				(m_sound._handle());
	return				(m_sound.get_params()->freq);
}

IC	const f32 CScriptSound::GetMinDistance() const
{
	VERIFY				(m_sound._handle());
	return				(m_sound.get_params()->min_distance);
}

IC	const f32 CScriptSound::GetMaxDistance() const
{
	VERIFY				(m_sound._handle());
	return				(m_sound.get_params()->max_distance);
}

IC	const f32	CScriptSound::GetVolume		() const
{
	VERIFY				(m_sound._handle());
	return				(m_sound.get_params()->volume);
}

IC	bool CScriptSound::IsPlaying			() const
{
	VERIFY				(m_sound._handle());
	return				(!!m_sound._feedback());
}

IC	void CScriptSound::Stop					()
{
	VERIFY				(m_sound._handle());
	m_sound.stop		();
}

IC	void CScriptSound::StopDeffered			()
{
	VERIFY				(m_sound._handle());
	m_sound.stop_deffered();
}

IC	void CScriptSound::SetPosition			(const fVector3& position)
{
	VERIFY				(m_sound._handle());
	m_sound.set_position(position);
}

IC	void CScriptSound::SetFrequency			(f32 frequency)
{
	VERIFY				(m_sound._handle());
	m_sound.set_frequency(frequency);
}

IC	void CScriptSound::SetVolume			(f32 volume)
{
	VERIFY				(m_sound._handle());
	m_sound.set_volume	(volume);
}

IC	const CSound_params *CScriptSound::GetParams	()
{
	VERIFY				(m_sound._handle());
	return				(m_sound.get_params());
}

IC	void CScriptSound::SetParams			(CSound_params *sound_params)
{
	VERIFY				(m_sound._handle());
	m_sound.set_params	(sound_params);
}
