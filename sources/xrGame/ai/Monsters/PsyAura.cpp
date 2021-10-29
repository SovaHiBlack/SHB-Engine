#include "stdafx.h"

#include "PsyAura.h"
#include "BaseMonster/BaseMonster.h"

CPsyAura::CPsyAura( )
{
	m_object = nullptr;
	m_radius = 1.0f;
}

CPsyAura::~CPsyAura( )
{ }

void CPsyAura::schedule_update( )
{
	inherited::schedule_update( );

	if (is_active( ))
	{
		feel_touch_update(m_object->Position( ), m_radius);
		process_objects_in_aura( );
	}
}
