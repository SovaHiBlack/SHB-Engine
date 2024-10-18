#pragma once
#include "control_combase.h"


class CControlDirectionBase : public CControl_ComBase {
	typedef CControl_ComBase inherited;

	u32				m_time_last_faced;
	u32				m_delay;

public:	
	struct SAxis {
		f32	target;
		f32	acceleration;
		f32	speed_target;

		void	init	() {
			target			= 0;
			speed_target	= 0;
			acceleration	= flt_max;
		}
	} m_heading, m_pitch;

public:
	virtual void	reinit					();
	virtual void	update_frame			();

			void	use_path_direction		(bool reversed = false);

	virtual	void	face_target				(const Fvector &position,	u32 delay = 0, f32 add_yaw = 0.0f);
	virtual	void	face_target				(const CObject *obj,		u32 delay = 0, f32 add_yaw = 0.0f);
	IC		void	set_delay				(u32 delay) {m_delay = delay;}

			void	set_heading_speed		(f32 value, bool force = false);
			void	set_heading				(f32 value, bool force = false);

	IC	const	SAxis	&heading	() {return m_heading;}	
};
