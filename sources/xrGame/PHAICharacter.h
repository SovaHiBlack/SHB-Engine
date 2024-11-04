#pragma once

#include "PHSimpleCharacter.h"

class CPHAICharacter : public CPHSimpleCharacter
{
	typedef CPHSimpleCharacter	inherited;

	fVector3 m_vDesiredPosition;
	bool	m_forced_physics_control;

public:
							CPHAICharacter						();
	virtual CPHAICharacter	*CastAICharacter					()																					{return this;}
	virtual		void		SetPosition							(fVector3 pos);
	virtual		void		SetDesiredPosition					(const fVector3& pos)																{m_vDesiredPosition.set(pos)	;}
	virtual		void		GetDesiredPosition					(fVector3& dpos)																		{dpos.set(m_vDesiredPosition)	;}
	virtual		void		ValidateWalkOn						()																													;
	virtual		void		BringToDesired						(f32 time, f32 velocity, f32 force=1.f)																			;
	virtual		bool		TryPosition							(fVector3 pos,bool exact_state)																										;
	virtual		void		Jump								(const fVector3& jump_velocity)																						;
	virtual		void		SetMaximumVelocity					(dReal vel)																					{m_max_velocity=vel		;}
	virtual		void		InitContact							(dContact* c,bool	&do_collide,u16 material_idx_1,u16 material_idx_2)										;
	virtual		void		SetForcedPhysicsControl				(bool v){m_forced_physics_control=v;}
	virtual		bool		ForcedPhysicsControl				(){return m_forced_physics_control;}
	virtual		void		Create								(dVector3 sizes)																									;

#ifdef DEBUG
	virtual		void		OnRender							()																													;
#endif

};
