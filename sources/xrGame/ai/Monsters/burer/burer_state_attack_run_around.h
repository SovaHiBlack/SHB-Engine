#pragma once
#include "../state.h"

template<typename _Object>
class	CStateBurerAttackRunAround : public CState<_Object> {
	typedef CState<_Object>	inherited;

	fVector3				selected_point;
	u32					time_started;

	fVector3				dest_direction;

public:
						CStateBurerAttackRunAround	(_Object *obj);
	virtual void		initialize					();
	virtual void		execute						();

	virtual bool		check_start_conditions		();
	virtual bool		check_completion			();
};

#include "burer_state_attack_run_around_inline.h"