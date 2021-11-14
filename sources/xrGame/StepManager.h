#pragma once

#include "StepManager_defs.h"

class CEntityAlive;
class CBlend;

class C_StepManager
{
	unsigned char										m_legs_count;

	StepsMap								m_steps_map;
	S_StepInfo								m_step_info;

	CEntityAlive* m_object;

	U16										m_foot_bones[MAX_LEGS_COUNT];
	CBlend* m_blend;

	u32										m_time_anim_started;

public:
	C_StepManager( );
	virtual				~C_StepManager( );

	// init on construction
	virtual DLL_Pure* _construct( );
	virtual void		reload(const char* section);

	// call on set animation
	void				on_animation_start(MotionID motion_id, CBlend* blend);
	// call on updateCL
	void				update( );

	// process event
	virtual void		event_on_step( )
	{ }

protected:
	Fvector3			get_foot_position(E_LegType leg_type);
	virtual bool		is_on_ground( )
	{
		return true;
	}

private:
	void				reload_foot_bones( );
	void				load_foot_bones(CConfigurationFile::Sect& data);

	float				get_blend_time( );
};
