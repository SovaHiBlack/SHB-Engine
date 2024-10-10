#pragma once
#include "pp_effector_custom.h"

//////////////////////////////////////////////////////////////////////////
// CPPEffectorDistance
//////////////////////////////////////////////////////////////////////////
class CPPEffectorDistance : public CPPEffectorController {
	typedef CPPEffectorController inherited;

	f32			m_r_min_perc;		// min_radius (percents [0..1])
	f32			m_r_max_perc;		// max_radius (percents [0..1])
	f32			m_radius;
	f32			m_dist;
public:
	virtual void	load					(pcstr section);
	IC		void	set_radius				(f32 r) {m_radius = r;}
	IC		void	set_current_dist		(f32 dist) {m_dist = dist;}

	virtual bool	check_completion		();
	virtual bool	check_start_conditions	();
	virtual void	update_factor			();

	virtual CPPEffectorControlled *create_effector	();
};
