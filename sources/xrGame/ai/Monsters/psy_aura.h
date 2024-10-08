#pragma once
#include "../../..\XR_3DA\feel_touch.h"
#include "energy_holder.h"

class CBaseMonster;

class CPsyAura : public Feel::Touch, public CEnergyHolder {
	
	typedef CEnergyHolder inherited;

	// владелец поля
	CBaseMonster	*m_object;

	// радиус поля
	F32			m_radius;

public:
					CPsyAura				();
	virtual			~CPsyAura				();
	
			void	init_external			(CBaseMonster *obj) {m_object = obj;}
	virtual BOOL	feel_touch_contact		(CObject* O){return FALSE;}
	virtual	void	schedule_update			();
	virtual void	process_objects_in_aura	() {}

	// свойства поля
			void	set_radius				(F32 R) {m_radius = R;}
			F32	get_radius				(){return m_radius;}

	CBaseMonster	*get_object				(){return m_object;}
};



