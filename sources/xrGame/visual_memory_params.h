////////////////////////////////////////////////////////////////////////////
//	Module 		: visual_memory_params.h
//	Created 	: 09.12.2004
//  Modified 	: 09.12.2004
//	Author		: Dmitriy Iassenev
//	Description : Visual memory parameters
////////////////////////////////////////////////////////////////////////////

#pragma once

struct CVisionParameters {
	f32						m_min_view_distance;
	f32						m_max_view_distance;
	f32						m_visibility_threshold;
	f32						m_always_visible_distance;
	f32						m_time_quant;
	f32						m_decrease_value;
	f32						m_velocity_factor;
	f32						m_transparency_threshold;
	f32						m_luminocity_factor;
	u32							m_still_visible_time;

			void	Load		(pcstr section, bool not_a_stalker);
};
