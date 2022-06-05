#pragma once

#include "control_combase.h"

struct SControlDirectionData : public ControlCom::IComData {
	struct {
		F32	target_angle;
		F32	target_speed;
	} heading, pitch;

	bool		linear_dependency;
};

struct SRotationEventData : public ControlCom::IEventData {

	enum RotType {
		eHeading		= u32(1) << 0,
		ePitch			= u32(1) << 1,
	};
	u8 angle;
};

class CControlDirection : public CControl_ComPure<SControlDirectionData> {
	typedef CControl_ComPure<SControlDirectionData> inherited;

	struct {
		F32	current_angle;
		F32	current_speed;			// current speed
		F32	current_acc;

		void	init	() {
			current_angle	= 0;
			current_speed	= 0;
			current_acc		= flt_max;
		}
	} m_heading, m_pitch;

public:

	virtual void	reinit				();
	virtual void	update_frame		();
	
	// services
			bool	is_face_target		(const Fvector &position, F32 eps_angle);
			bool	is_face_target		(const CObject *obj, F32 eps_angle);

			bool	is_from_right		(const Fvector &position);
			bool	is_from_right		(F32 yaw);

			bool	is_turning			(F32 eps_angle = EPS);

			void	get_heading			(F32& current, F32& target);
			F32	get_heading_current	();

			F32	angle_to_target		(const Fvector &position);
private:				
			void	pitch_correction	();
};
