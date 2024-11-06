#pragma once

#include "control_combase.h"

struct SControlDirectionData : public ControlCom::IComData {
	struct {
		f32	target_angle;
		f32	target_speed;
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
		f32	current_angle;
		f32	current_speed;			// current speed
		f32	current_acc;

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
	bool	is_face_target(const fVector3& position, f32 eps_angle);
			bool	is_face_target		(const CObject *obj, f32 eps_angle);

			bool	is_from_right(const fVector3& position);
			bool	is_from_right		(f32 yaw);

			bool	is_turning			(f32 eps_angle = EPSILON_5);

			void	get_heading			(f32& current, f32& target);
			f32	get_heading_current	();

			f32	angle_to_target(const fVector3& position);

private:				
			void	pitch_correction	();
};
