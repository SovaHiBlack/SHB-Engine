#pragma once
#include "control_combase.h"
#include "..\..\..\XR_3DA\SkeletonAnimated.h"

struct SControlRotationJumpData : public ControlCom::IComData {
	MotionID		anim_stop_ls,anim_run_ls;
	MotionID		anim_stop_rs,anim_run_rs;
	f32			turn_angle;

	enum EFlags {	
		eStopAtOnce	= u32(1) << 0,  // stop at once
		eRotateOnce	= u32(1) << 1,	// use only the first stage
	};

	flags32			flags;
};

class CControlRotationJump : public CControl_ComCustom<SControlRotationJumpData> {
	typedef CControl_ComCustom<SControlRotationJumpData> inherited;
	u32				m_time_next_rotation_jump;
	
	f32			m_target_velocity;
	f32			m_start_velocity;
	f32			m_accel;
	f32			m_dist;
	f32			m_time;

	bool			m_right_side;

	enum EStage {
		eStop,
		eRun, 
		eNone
	} m_stage;

	CKinematicsAnimated	*m_skeleton_animated;

public:
	virtual void	reinit					();

	virtual void	on_event				(ControlCom::EEventType, ControlCom::IEventData*);
	virtual void	activate				();
	virtual void	on_release				();
	virtual bool	check_start_conditions	();

private:	
			void	build_line_first		();
			void	build_line_second		();
			void	stop_at_once			();
};
