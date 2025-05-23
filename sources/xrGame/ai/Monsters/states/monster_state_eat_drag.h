#pragma once

template<typename _Object>
class CStateMonsterDrag : public CState<_Object> {
	typedef CState<_Object>		inherited;

	fVector3				m_cover_position;
	u32					m_cover_vertex_id;
	
	bool				m_failed;
	fVector3				m_corpse_start_position;

public:
						CStateMonsterDrag		(_Object *obj);
	virtual				~CStateMonsterDrag		();

	virtual void		initialize				();
	virtual	void		execute					();
	virtual void		finalize				();
	virtual void		critical_finalize		();

	virtual bool		check_completion		();
};

#include "monster_state_eat_drag_inline.h"
