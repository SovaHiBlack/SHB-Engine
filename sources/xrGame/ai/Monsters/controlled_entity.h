#pragma once
#include "Controller/Controller.h"

class CEntity;

enum ETask {
	eTaskFollow		= u32(0),
	eTaskAttack,
	eTaskNone		= u32(-1)
};

struct SControlledInfo {
	ETask			m_task;
	const CEntity	*m_object;
	fVector3			m_position;
	u32				m_node;		
	f32			m_radius;
};

class CControlledEntityBase {
public:
	virtual bool			is_under_control	()								= 0;

	virtual void			set_data			(const SControlledInfo &info)	= 0;
	virtual SControlledInfo &get_data			()								= 0;

	virtual void			set_task_follow		(const CEntity *e)				= 0;
	virtual void			set_task_attack		(const CEntity *e)				= 0;
	
	virtual void			set_under_control	(CController *controller)		= 0;
	virtual void			free_from_control	()								= 0;

	virtual void			on_reinit			()								= 0;
	virtual void			on_die				()								= 0;
	virtual void			on_destroy			()								= 0;
};


template<typename _Object>
class CControlledEntity : public CControlledEntityBase {
	
	SControlledInfo		m_data;
	
	struct SGroupID {
		s32 team_id;
		s32 squad_id;
		s32 group_id;
	} saved_id;

	_Object				*m_object;
	CController			*m_controller;

public:

	virtual bool			is_under_control		() {return (m_controller != 0);}

	virtual void			set_data				(const SControlledInfo &info) {m_data = info;}
	virtual SControlledInfo &get_data				(){return m_data;}

	virtual void			set_task_follow			(const CEntity *e);
	virtual void			set_task_attack			(const CEntity *e);

	virtual void			set_under_control		(CController *controller); 
	virtual void			free_from_control		();

	virtual void			on_reinit				();
	virtual void			on_die					();
	virtual void			on_destroy				();

			void			init_external			(_Object *obj) {m_object = obj;}
			
			CController		*get_controller			() {return m_controller;}
};

#include "controlled_entity_inline.h"

