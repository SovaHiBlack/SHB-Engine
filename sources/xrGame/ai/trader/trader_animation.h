#pragma once

#include "..\..\..\XR_3DA\SkeletonAnimated.h"

class CAI_Trader;

namespace MonsterSpace {
	enum EMonsterHeadAnimType;
};

class CTraderAnimation {
	CAI_Trader			*m_trader;

	pcstr				m_anim_global;
	pcstr				m_anim_head;
	
	MotionID			m_motion_head;
	MotionID			m_motion_global;

	ref_sound			*m_sound;

	bool				m_external_sound;

public:
					CTraderAnimation		(CAI_Trader *trader) : m_trader(trader) {}

	void			reinit					();

	void			set_animation			(pcstr anim);
	void			set_head_animation		(pcstr anim);
	void			set_sound				(pcstr sound, pcstr head_anim);

	// Callbacks
	static void		global_callback			(CBlend* B);
	static void		head_callback			(CBlend* B);

	void			update_frame			();

	void			external_sound_start	(pcstr phrase);
	void			external_sound_stop		();

private:
	void			remove_sound			();
};
