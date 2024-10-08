#pragma once

template<typename _Object>
class CScanningAbility {
	_Object		*object;

	// external members
	F32		critical_value;
	F32		scan_radius;
	F32		velocity_threshold;
	F32		decrease_value;
	F32		scan_trace_time_freq;

	ref_sound	sound_scan;

	// internal members
	enum EScanState {
		eStateDisabled,
		eStateNotActive,
		eStateScanning
	} state;

	F32		scan_value;

	SPPInfo		m_effector_info;
	F32		m_effector_time;
	F32		m_effector_time_attack;
	F32		m_effector_time_release;

	u32			time_last_trace;

	bool		m_this_scan;

public:
			void	init_external			(_Object *obj) {object = obj;}
			void	on_destroy				();

			void	load					(pcstr section);
	virtual	void	reinit					();
		
			void	schedule_update			();
			void	frame_update			(u32 dt);

			void	enable					();
			void	disable					();

	virtual void	on_scan_success			() {}
	virtual void	on_scanning				() {}

private:
	F32	get_velocity			(CObject *obj);

#ifdef DEBUG
public:
	F32	get_scan_value			(){return scan_value;}
#endif

};

#include "scanning_ability_inline.h"