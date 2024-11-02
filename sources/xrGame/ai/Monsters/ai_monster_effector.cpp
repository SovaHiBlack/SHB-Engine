#include "stdafx.h"
#include "ai_monster_effector.h"

#define MONSTER_EFFECTOR_TYPE_ID 5

//////////////////////////////////////////////////////////////////////////
// CMonsterEffector
//////////////////////////////////////////////////////////////////////////
CMonsterEffector::CMonsterEffector(const SPPInfo &ppi, f32 life_time, f32 attack_time, f32 release_time, f32 spec_factor) :
		CEffectorPP(EEffectorPPType(eCEHit), life_time)
{
	state		= ppi;
	m_total		= life_time;
	
	m_attack	= ((fis_zero(attack_time)) ? 0.5f : attack_time);
	m_release	= ((fis_zero(release_time)) ? 0.5f : release_time);

	m_spec_factor = spec_factor;

	VERIFY(!fsimilar(m_release, 1.0f));
	VERIFY(!fis_zero(m_attack));
}

BOOL CMonsterEffector::Process(SPPInfo& pp)
{
	inherited::Process(pp);

	// amount of time passed in percents
	f32 time_past_perc = (m_total - fLifeTime) / m_total;

	f32 factor;
	if (time_past_perc < m_attack) 
		factor = time_past_perc / m_attack;
	else if ((time_past_perc >= m_attack) && (time_past_perc <= m_release)) 
		factor = 1.0f;
	else 
		factor = (1.0f - time_past_perc) / (1.0f - m_release);

	clamp(factor,0.01f,1.0f);

	pp.lerp				(pp_identity, state, factor * m_spec_factor);
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// CMonsterEffectorHit
//////////////////////////////////////////////////////////////////////////

CMonsterEffectorHit::CMonsterEffectorHit(f32 time, f32 amp, f32 periods, f32 power)
	: CEffectorCam(eCEMonsterHit, time)
{
	total			= time;

	max_amp			= amp * power;
	period_number	= periods;
	this->power		= power;

	offset.set		(Random.randF(1,2),Random.randF(1,6),Random.randF(1,6));
}

BOOL CMonsterEffectorHit::Process(fVector3& p, fVector3& d, fVector3& n, f32& fFov, f32& fFar, f32& fAspect)
{
	fLifeTime -= Device.fTimeDelta; if(fLifeTime<0) return FALSE;

	// процент оставшегося времени
	f32 time_left_perc = fLifeTime / total;

	// Инициализация
	fMatrix4x4	Mdef;
	Mdef.identity		();
	Mdef.j.set			(n);
	Mdef.k.set			(d);
	Mdef.i.crossproduct	(n,d);
	Mdef.c.set			(p);

	f32 period_all	= period_number * PI_MUL_2;		// макс. значение цикла
	f32 cur_amp		= max_amp * (PI / 180) * time_left_perc;

	fVector3 dangle;
	dangle.x = cur_amp/offset.x	* _sin(period_all/offset.x	* (1.0f - time_left_perc));
	dangle.y = cur_amp/offset.y	* _cos(period_all/offset.y	* (1.0f - time_left_perc));
	dangle.z = cur_amp/offset.z	* _sin(period_all/offset.z	* (1.0f - time_left_perc));

	// Установить углы смещения
	fMatrix4x4		R;
	R.setHPB	(dangle.x,dangle.y,dangle.z);

	fMatrix4x4		mR;
	mR.mul		(Mdef,R);

	d.set		(mR.k);
	n.set		(mR.j);

	return TRUE;
}
