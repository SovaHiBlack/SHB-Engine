#pragma once
#include "../control_combase.h"
#include "../../../..\XR_3DA\SkeletonAnimated.h"

class CControllerPsyHit : public CControl_ComCustom<> {
	typedef CControl_ComCustom<> inherited;

	MotionID			m_stage[4];
	u8					m_current_index;

	enum ESoundState{
		ePrepare,
		eStart,
		ePull,
		eHit,
		eNone
	} m_sound_state;

	f32				m_min_tube_dist;

	// internal flag if weapon was hidden
	bool				m_blocked;

public:
	virtual void	load					(pcstr section);
	virtual	void	reinit					();
	virtual	void	update_frame			();
	virtual bool	check_start_conditions	();
	virtual void	activate				();
	virtual void	deactivate				();
	
	virtual void	on_event				(ControlCom::EEventType, ControlCom::IEventData*);

			void	on_death				();

private:
			void	play_anim				();
			void	death_glide_start			();
			void	death_glide_end			();

			void	set_sound_state			(ESoundState state);
			void	hit						();
			bool	check_conditions_final	();
};
