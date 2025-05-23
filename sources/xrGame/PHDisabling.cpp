#include "stdafx.h"
#include "PHDisabling.h"
#include "PhysicsCommon.h"
#include "Physics.h"

extern CPHWorld* ph_world;

SDisableVector::SDisableVector( )
{
	Init( );
}

void	SDisableVector::Reset( )
{
	sum.set(0.f, 0.f, 0.f);
}

void	SDisableVector::Init( )
{
	previous.set(0.0f, 0.0f, 0.0f);
	Reset( );
}

f32	SDisableVector::Update(const fVector3& new_vector)
{
	fVector3 dif;
	dif.sub(new_vector, previous);
	previous.set(new_vector);
	sum.add(dif);
	return dif.magnitude( );
}

f32	SDisableVector::UpdatePrevious(const fVector3& new_vector)
{
	fVector3 dif;
	dif.sub(new_vector, previous);
	previous.set(new_vector);
	return dif.magnitude( );
}

f32	SDisableVector::SumMagnitude( )
{
	return sum.magnitude( );
}

SDisableUpdateState::SDisableUpdateState( )
{
	Reset( );
}

void	SDisableUpdateState::Reset( )
{
	disable = false;
	enable = false;
}

SDisableUpdateState& SDisableUpdateState::operator &=	(SDisableUpdateState& lstate)
{
	disable = disable && lstate.disable;
	enable = enable || lstate.enable;
	return *this;
}

CBaseDisableData::CBaseDisableData( )
{
	m_frames = worldDisablingParams.objects_params.L2frames;
	Reinit( );
}

void	CBaseDisableData::Reinit( )
{
	m_count = m_frames;
	if (ph_world)
		m_count = m_count + ph_world->disable_count;
	m_stateL1.Reset( );
	m_stateL2.Reset( );
}

void	CBaseDisableData::Disabling( )
{
	dBodyID	body = get_body( );
	m_count--;

	UpdateL1( );

	CheckState(m_stateL1);

	if (m_count == 0)//ph_world->disable_count==dis_frames//m_count==m_frames
	{
		UpdateL2( );
		CheckState(m_stateL2);
		m_count = m_frames;
	}
	const	dReal* force = dBodyGetForce(body);
	const	dReal* torqu = dBodyGetTorque(body);
	if (dDOT(force, force) > 0.f || dDOT(torqu, torqu) > 0.f) m_disabled = false;
	if (dBodyIsEnabled(body))
	{
		ReEnable( );
		if (!m_disabled && (ph_world->disable_count != m_count % worldDisablingParams.objects_params.L2frames))
		{
			m_count = m_frames + ph_world->disable_count;
		}
	}

	if (m_disabled)
		Disable( );//dBodyDisable(body);
}

void CPHDisablingBase::Reinit( )
{
	m_mean_velocity.Init( );
	m_mean_acceleration.Init( );
	CBaseDisableData::Reinit( );
	bool disable = !dBodyIsEnabled(get_body( ));
	m_stateL1.disable = disable;
	m_stateL1.enable = !disable;
	m_stateL2.disable = disable;
	m_stateL2.enable = !disable;
}

void CPHDisablingBase::UpdateValues(const fVector3& new_pos, const fVector3& new_vel)
{
	if (m_count < m_frames)
	{
		f32 velocity_param = m_mean_velocity.Update(new_pos);
		f32 acceleration_param = m_mean_acceleration.Update(new_vel);
		CheckState(m_stateL1, velocity_param * m_frames, acceleration_param * m_frames);
	}
	else
	{
		f32 velocity_param = m_mean_velocity.UpdatePrevious(new_pos);
		f32 acceleration_param = m_mean_acceleration.UpdatePrevious(new_vel);
		CheckState(m_stateL1, velocity_param * m_frames, acceleration_param * m_frames);
	}
}

void CPHDisablingBase::UpdateL2( )
{
	m_stateL2.Reset( );
	f32 velocity_param = m_mean_velocity.SumMagnitude( ) / m_frames;
	f32 acceleration_param = m_mean_acceleration.SumMagnitude( ) / m_frames;

	CheckState(m_stateL2, velocity_param, acceleration_param);

	m_mean_velocity.Reset( );
	m_mean_acceleration.Reset( );
}

void CPHDisablingBase::set_DisableParams(const SOneDDOParams& params)
{
	m_params = params;
}

CPHDisablingTranslational::CPHDisablingTranslational( )
{
	m_params = worldDisablingParams.objects_params.translational;
}

void CPHDisablingTranslational::Reinit( )
{
	CPHDisablingBase::Reinit( );
	dBodyID				body = get_body( );
	const	dReal* position = dBodyGetPosition(body);
	const	dReal* velocity = dBodyGetLinearVel(body);
	m_mean_velocity.UpdatePrevious(*(fVector3*)position);
	m_mean_acceleration.UpdatePrevious(*(fVector3*)velocity);
}

void	CPHDisablingTranslational::UpdateL1( )
{
	m_stateL1.Reset( );
	dBodyID			body = get_body( );
	const	dReal* position = dBodyGetPosition(body);
	const	dReal* velocity = dBodyGetLinearVel(body);

	CPHDisablingBase::UpdateValues(*(fVector3*)position, *(fVector3*)velocity);
	//f32			velocity_param		=	m_mean_velocity		.Update(* (fVector3*) position)		;
	//f32			acceleration_param	=	m_mean_acceleration	.Update(* (fVector3*) velocity)		;
	//CheckState						(m_stateL1,velocity_param*m_frames,acceleration_param*m_frames)					;
}

void CPHDisablingTranslational::set_DisableParams(const SAllDDOParams& params)
{
	CPHDisablingBase::set_DisableParams(params.translational);
	m_frames = params.L2frames;
}

CPHDisablingRotational::CPHDisablingRotational( )
{
	m_params = worldDisablingParams.objects_params.rotational;
}

void	CPHDisablingRotational::Reinit( )
{
	CPHDisablingBase::Reinit( );
	dBodyID			body = get_body( );
	const	dReal* rotation = dBodyGetRotation(body);
	const	dReal* velocity = dBodyGetAngularVel(body);
	fVector3	vrotation;
	vrotation.set(rotation[9], rotation[2], rotation[4]);
	m_mean_velocity.UpdatePrevious(vrotation);
	m_mean_acceleration.UpdatePrevious(*(fVector3*)velocity);
}

void CPHDisablingRotational::UpdateL1( )
{
	m_stateL1.Reset( );
	dBodyID body = get_body( );
	const dReal* rotation = dBodyGetRotation(body);
	const dReal* velocity = dBodyGetAngularVel(body);
	fVector3 vrotation;
	vrotation.set(rotation[9], rotation[2], rotation[4]);

	CPHDisablingBase::UpdateValues(vrotation, *(fVector3*)velocity);
	//f32			velocity_param		=	m_mean_velocity		.Update	(			 vrotation	)	;
	//f32			acceleration_param	=	m_mean_acceleration	.Update	(* (fVector3*) velocity	)	;

	//CheckState									(m_stateL1,velocity_param,acceleration_param)		;
}

void CPHDisablingRotational::set_DisableParams(const SAllDDOParams& params)
{
	CPHDisablingBase::set_DisableParams(params.rotational);
	m_frames = params.L2frames;
}

void CPHDisablingFull::Reinit( )
{
	CPHDisablingRotational::Reinit( );
	CPHDisablingTranslational::Reinit( );
}

void CPHDisablingFull::UpdateL1( )
{
	SDisableUpdateState state;
	CPHDisablingRotational::UpdateL1( );
	state = m_stateL1;
	CPHDisablingTranslational::UpdateL1( );
	m_stateL1 &= state;
}

void CPHDisablingFull::UpdateL2( )
{
	SDisableUpdateState state;
	CPHDisablingRotational::UpdateL2( );
	state = m_stateL2;
	CPHDisablingTranslational::UpdateL2( );
	m_stateL2 &= state;
}

void CPHDisablingFull::set_DisableParams(const SAllDDOParams& params)
{
	CPHDisablingRotational::set_DisableParams(params);
	CPHDisablingTranslational::set_DisableParams(params);
}
