//	Module 		: stalker_animation_pair_inline.h
//	Description : Stalker animation pair inline functions

#pragma once

inline	CStalkerAnimationPair::CStalkerAnimationPair	()
{
#ifdef DEBUG
	m_object_name				= "unassigned";
	m_animation_type_name		= "unassigned";
#endif
	reset						();
	m_step_dependence			= false;
	m_global_animation			= false;
}

inline	void CStalkerAnimationPair::reset				()
{
#if 0//def DEBUG
	if (m_animation)
		Msg						("animation [%s][%s] is reset",m_object_name,m_animation_type_name);
#endif // DEBUG

	m_animation.invalidate		();
	m_blend						= 0;
	m_actual					= true;
	m_array						= 0;
	m_array_animation.invalidate();
}

inline	bool CStalkerAnimationPair::actual				() const
{
	return						(m_actual);
}

inline	bool CStalkerAnimationPair::animation			(const MotionID &animation)
{
	bool						result = (m_animation == animation);
	m_actual					= m_actual && result;
	m_animation					= animation;
	return						(result);
}

inline	const MotionID &CStalkerAnimationPair::animation() const
{
	return						(m_animation);
}

inline	CBlend *CStalkerAnimationPair::blend			() const
{
	return						(m_blend);
}

inline	void CStalkerAnimationPair::step_dependence		(bool value)
{
	m_step_dependence			= value;
}

inline	bool CStalkerAnimationPair::step_dependence		() const
{
	return						(m_step_dependence);
}

#ifdef DEBUG
inline	void CStalkerAnimationPair::set_dbg_info		(const char* object_name, const char* animation_type_name)
{
	m_object_name				= object_name;
	m_animation_type_name		= animation_type_name;
}
#endif

inline	void CStalkerAnimationPair::global_animation	(bool global_animation)
{
	m_global_animation			= global_animation;
}

inline	bool CStalkerAnimationPair::global_animation	() const
{
	return						(m_global_animation);
}

inline	void CStalkerAnimationPair::make_inactual		()
{
	m_actual					= false;
}

inline	const CStalkerAnimationPair::CALLBACK_ID *CStalkerAnimationPair::callback	(const CALLBACK_ID &callback) const
{
	CALLBACKS::const_iterator	I = std::find(m_callbacks.begin(),m_callbacks.end(),callback);
	if (I != m_callbacks.end())
		return					(&*I);

	return						(0);
}

inline	void CStalkerAnimationPair::add_callback		(const CALLBACK_ID &callback)
{
	VERIFY						(std::find(m_callbacks.begin(),m_callbacks.end(),callback) == m_callbacks.end());
	m_callbacks.push_back		(callback);
}

inline	void CStalkerAnimationPair::remove_callback		(const CALLBACK_ID &callback)
{
	CALLBACKS::iterator			I = std::find(m_callbacks.begin(),m_callbacks.end(),callback);
	VERIFY						(I != m_callbacks.end());
	m_callbacks.erase			(I);
}

inline	bool CStalkerAnimationPair::need_update			() const
{
	return						(!m_callbacks.empty());
}
