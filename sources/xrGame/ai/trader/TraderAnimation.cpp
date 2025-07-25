#include "stdafx.h"

#include "Trader.h"
#include "TraderAnimation.h"
#include "../../script_callback_ex.h"
#include "../../GameObject_space.h"

/////////////////////////////////////////////////////////////////////////////////////////
// Startup
/////////////////////////////////////////////////////////////////////////////////////////
void CTraderAnimation::reinit() {
	m_motion_head.invalidate	();
	m_motion_global.invalidate	();
	m_sound						= 0;
	m_external_sound			= 0;

	m_anim_global				= 0;
	m_anim_head					= 0;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Animation Callbacks
/////////////////////////////////////////////////////////////////////////////////////////
void CTraderAnimation::global_callback(CBlend* B)
{
	CTraderAnimation *trader			= (CTraderAnimation*)B->CallbackParam;
	trader->m_motion_global.invalidate	();
}

void CTraderAnimation::head_callback(CBlend* B)
{
	CTraderAnimation *trader			= (CTraderAnimation*)B->CallbackParam;
	trader->m_motion_head.invalidate	();
}

/////////////////////////////////////////////////////////////////////////////////////////
// Animation management
/////////////////////////////////////////////////////////////////////////////////////////
void CTraderAnimation::set_animation(pcstr anim)
{
	m_anim_global								= anim;

	CKinematicsAnimated	*kinematics_animated	= smart_cast<CKinematicsAnimated*>(m_trader->Visual());
	m_motion_global								= kinematics_animated->ID_Cycle(m_anim_global);
	kinematics_animated->PlayCycle				(m_motion_global,TRUE,global_callback,this);
}

void CTraderAnimation::set_head_animation(pcstr anim)
{
	m_anim_head = anim;

	// ��������� �������� ������
	CKinematicsAnimated	*kinematics_animated	= smart_cast<CKinematicsAnimated*>(m_trader->Visual());
	m_motion_head								= kinematics_animated->ID_Cycle(m_anim_head);
	kinematics_animated->PlayCycle				(m_motion_head,TRUE,head_callback,this);	
}

//////////////////////////////////////////////////////////////////////////
// Sound management
//////////////////////////////////////////////////////////////////////////
void CTraderAnimation::set_sound(pcstr sound, pcstr anim)
{
	if (m_sound)		remove_sound();	
	
	set_head_animation	(anim);

	m_sound				= xr_new<ref_sound>();
	m_sound->create		(sound,st_Effect, eST_WORLD);
	m_sound->play		(NULL, sm_2D);
}

void CTraderAnimation::remove_sound()
{
	VERIFY				(m_sound);
	
	if (m_sound->_feedback()) 
						m_sound->stop();
	
	m_sound->destroy	();
	xr_delete			(m_sound);
}

//////////////////////////////////////////////////////////////////////////
// Update
//////////////////////////////////////////////////////////////////////////
void CTraderAnimation::update_frame()
{
	if (m_sound && !m_sound->_feedback()) {
		m_trader->callback	(GameObject::eTraderSoundEnd)();
		remove_sound		();
	}

	if (!m_motion_global) {
		m_trader->callback(GameObject::eTraderGlobalAnimationRequest)();
		if (m_anim_global) m_motion_head.invalidate();
	}

	// ��������� �������� ������
	if (!m_motion_head) {
		if (m_sound && m_sound->_feedback()) {
			m_trader->callback(GameObject::eTraderHeadAnimationRequest)();
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// External sound support
//////////////////////////////////////////////////////////////////////////
void CTraderAnimation::external_sound_start(pcstr phrase)
{
	if (m_sound)
	{
		remove_sound( );
	}
	
	m_sound					= xr_new<ref_sound>();
	m_sound->create			(phrase,st_Effect, eST_WORLD);
	m_sound->play			(NULL, sm_2D);

	m_motion_head.invalidate();
}

void CTraderAnimation::external_sound_stop()
{
	if (m_sound)
	{
		remove_sound( );
	}
}
//////////////////////////////////////////////////////////////////////////
