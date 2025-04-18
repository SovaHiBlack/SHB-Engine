#include "stdafx.h"
#include "bloodsucker_vampire_effector.h"

//////////////////////////////////////////////////////////////////////////
// Vampire Postprocess Effector
//////////////////////////////////////////////////////////////////////////

CVampirePPEffector::CVampirePPEffector(const SPPInfo &ppi, f32 life_time) :
	inherited(EEffectorPPType(eCEHit), life_time)
{
	state		= ppi;
	m_total		= life_time;
}

#define TIME_ATTACK		0.2f
#define PERIODS			2			
#define RAD_TO_PERC(rad)	((rad - PI_DIV_2) / (PERIODS * PI_MUL_2))
#define PERC_TO_RAD(perc)	(perc * (PERIODS * PI_MUL_2) + PI_DIV_2)

BOOL CVampirePPEffector::Process(SPPInfo& pp)
{
	inherited::Process(pp);

	// amount of time passed in percents
	f32 time_past_perc = (m_total - fLifeTime) / m_total;
	
	f32 factor;
	if (time_past_perc < TIME_ATTACK) {
		factor = 0.75f * time_past_perc / TIME_ATTACK;
	} else if (time_past_perc > (1 - TIME_ATTACK)) {
		factor = 0.75f * (1-time_past_perc) / TIME_ATTACK;
	} else {	
		f32 time_past_sine_perc = (time_past_perc - TIME_ATTACK) * (1 / ( 1 - TIME_ATTACK + TIME_ATTACK));
		factor = 0.5f + 0.25f * _sin(PERC_TO_RAD(time_past_sine_perc));
	}
	
	clamp(factor,0.01f,1.0f);
	pp.lerp				(pp_identity, state, factor);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// Vampire Camera Effector
//////////////////////////////////////////////////////////////////////////
#define DELTA_ANGLE_X	10 * PI / 180
#define DELTA_ANGLE_Y	DELTA_ANGLE_X
#define DELTA_ANGLE_Z	DELTA_ANGLE_X
#define ANGLE_SPEED		0.2f	
#define BEST_DISTANCE	0.3f
CVampireCameraEffector::CVampireCameraEffector(f32 time, const fVector3& src, const fVector3& tgt) :
	inherited(eCEVampire, time)
{
	fLifeTime				= time;
	m_time_total			= time;
	
	m_dist					= src.distance_to(tgt);
	
	if (m_dist < BEST_DISTANCE)	{
		m_direction.sub		(src,tgt);
		m_dist = BEST_DISTANCE - m_dist;
	} else {
		m_direction.sub			(tgt,src);
		m_dist = m_dist - BEST_DISTANCE;
	}
	
	m_direction.normalize	();

	dangle_target.set	(Random.randFs(DELTA_ANGLE_X),Random.randFs(DELTA_ANGLE_Y),Random.randFs(DELTA_ANGLE_Z));
	dangle_current.set	(0.f, 0.f, 0.f);
}

BOOL CVampireCameraEffector::Process(fVector3& p, fVector3& d, fVector3& n, f32& fFov, f32& fFar, f32& fAspect)
{
	fLifeTime -= Device.fTimeDelta; if(fLifeTime<0) return FALSE;

	// ������� ����������� �������
	f32 time_left_perc = fLifeTime / m_time_total;

	// �������������
	fMatrix4x4	Mdef;
	Mdef.identity		();
	Mdef.j.set			(n);
	Mdef.k.set			(d);
	Mdef.i.crossproduct	(n,d);
	Mdef.c.set			(p);

	//////////////////////////////////////////////////////////////////////////
	// using formula: y = k - 2*k*abs(x-1/2)   k - max distance
	//f32	cur_dist = m_dist * (1 - 2*_abs((1-time_left_perc) - 0.5f));
	f32 time_passed	= 1-time_left_perc;
	f32 cur_dist		= m_dist * (_sqrt(0.5f*0.5f - (time_passed - 0.5f)*(time_passed - 0.5f)) );

	Mdef.c.mad(m_direction, cur_dist);
	
	// check the time to return
	if (time_left_perc < 0.2f) {

		dangle_target.x = 0.f;
		dangle_target.y = 0.f;
		dangle_target.z = 0.f;

		angle_lerp(dangle_current.x, dangle_target.x, _abs(dangle_current.x / fLifeTime + 0.001f), Device.fTimeDelta);
		angle_lerp(dangle_current.y, dangle_target.y, _abs(dangle_current.y / fLifeTime + 0.001f), Device.fTimeDelta);
		angle_lerp(dangle_current.z, dangle_target.z, _abs(dangle_current.z / fLifeTime + 0.001f), Device.fTimeDelta);

	} else {
		
		if (angle_lerp(dangle_current.x, dangle_target.x, ANGLE_SPEED, Device.fTimeDelta)) {
			dangle_target.x = Random.randFs(DELTA_ANGLE_X);
		}

		if (angle_lerp(dangle_current.y, dangle_target.y, ANGLE_SPEED, Device.fTimeDelta)) {
			dangle_target.y = Random.randFs(DELTA_ANGLE_Y);
		}

		if (angle_lerp(dangle_current.z, dangle_target.z, ANGLE_SPEED, Device.fTimeDelta)) {
			dangle_target.z = Random.randFs(DELTA_ANGLE_Z);
		}
	}

	//////////////////////////////////////////////////////////////////////////

	// ���������� ���� ��������
	fMatrix4x4		R;
	R.setHPB	(dangle_current.x,dangle_current.y,dangle_current.z);

	fMatrix4x4		mR;
	mR.mul		(Mdef,R);

	d.set		(mR.k);
	n.set		(mR.j);
	p.set		(mR.c);

	return TRUE;
}
