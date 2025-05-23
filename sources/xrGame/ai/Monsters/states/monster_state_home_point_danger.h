#pragma once
#include "../state.h"

template<typename _Object>
class CStateMonsterDangerMoveToHomePoint : public CState<_Object> {
protected:
	typedef CState<_Object>		inherited;
	typedef CState<_Object>*	state_ptr;

	u32					m_target_node;
	bool				m_skip_camp;
	fVector3				m_danger_pos;

public:
						CStateMonsterDangerMoveToHomePoint(_Object *obj);
	virtual	void		initialize				();
	virtual void 		finalize				();
	virtual void 		critical_finalize		();

	virtual bool		check_start_conditions	();
	virtual bool		check_completion		();

	virtual	void		reselect_state			();
	virtual	void		setup_substates			();

private:
	fVector3&			get_most_danger_pos	();
};

#include "monster_state_home_point_danger_inline.h"
