////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_stalker_inline.h
//	Created 	: 25.02.2003
//  Modified 	: 25.02.2003
//	Author		: Dmitriy Iassenev
//	Description : AI Behaviour for monster "Stalker" (inline functions)
////////////////////////////////////////////////////////////////////////////

#pragma once

inline BOOL CStalker::UsedAI_Locations( )
{
	return TRUE;
}

inline CStalkerAnimationManager& CStalker::animation( ) const
{
	VERIFY(m_animation_manager);
	return *m_animation_manager;
}

inline CStalkerPlanner& CStalker::brain( ) const
{
	VERIFY(m_brain);
	return *m_brain;
}

inline CSightManager& CStalker::sight( ) const
{
	VERIFY(m_sight_manager);
	return *m_sight_manager;
}

inline const char* CStalker::Name( ) const
{
	return CInventoryOwner::Name( );
}

inline CStalkerMovementManager& CStalker::movement( ) const
{
	VERIFY(m_movement_manager);
	return *m_movement_manager;
}

inline bool CStalker::frame_check(u32& frame)
{
	if (Device.dwFrame == frame)
	{
		return false;
	}

	frame = Device.dwFrame;
	return true;
}

inline bool CStalker::group_behaviour( ) const
{
	return m_group_behaviour;
}

inline CWeaponShotEffector& CStalker::weapon_shot_effector( ) const
{
	VERIFY(m_weapon_shot_effector);
	return *m_weapon_shot_effector;
}

inline u32 CStalker::min_queue_size_far( ) const
{
	return m_min_queue_size_far;
}

inline u32 CStalker::max_queue_size_far( ) const
{
	return m_max_queue_size_far;
}

inline u32 CStalker::min_queue_interval_far( ) const
{
	return m_min_queue_interval_far;
}

inline u32 CStalker::max_queue_interval_far( ) const
{
	return m_max_queue_interval_far;
}

inline u32 CStalker::min_queue_size_medium( ) const
{
	return m_min_queue_size_medium;
}

inline u32 CStalker::max_queue_size_medium( ) const
{
	return m_max_queue_size_medium;
}

inline u32 CStalker::min_queue_interval_medium( ) const
{
	return m_min_queue_interval_medium;
}

inline	u32 CStalker::max_queue_interval_medium( ) const
{
	return					(m_max_queue_interval_medium);
}

inline	u32 CStalker::min_queue_size_close( ) const
{
	return					(m_min_queue_size_close);
}

inline	u32 CStalker::max_queue_size_close( ) const
{
	return					(m_max_queue_size_close);
}

inline	u32 CStalker::min_queue_interval_close( ) const
{
	return					(m_min_queue_interval_close);
}

inline	u32 CStalker::max_queue_interval_close( ) const
{
	return					(m_max_queue_interval_close);
}

inline	bool CStalker::wounded( ) const
{
	return					(m_wounded);
}

inline	const CStalker::CRITICAL_WOUND_WEIGHTS& CStalker::critical_wound_weights( ) const
{
	VERIFY(!m_critical_wound_weights.empty( ));
	return					(m_critical_wound_weights);
}
