#include "stdafx.h"

#include "ActorEffector.h"
#include "PostprocessAnimator.h"
#include "..\XR_3DA\effectorPP.h"
#include "..\XR_3DA\ObjectAnimator.h"
#include "object_broker.h"
#include "Actor.h"

void AddEffector(CActor* A, s32 type, const shared_str& sect_name)
{
	if (pSettings->line_exist(sect_name, "pp_eff_name"))
	{
		bool bCyclic = !!pSettings->r_bool(sect_name, "pp_eff_cyclic");
		CPostprocessAnimator* pp_anm = xr_new<CPostprocessAnimator>( );
		pp_anm->SetType((EEffectorPPType) type);
		pp_anm->SetCyclic(bCyclic);

		pcstr fn = pSettings->r_string(sect_name, "pp_eff_name");
		pp_anm->Load(fn);
		A->Cameras( ).AddPPEffector(pp_anm);
	}

	if (pSettings->line_exist(sect_name, "cam_eff_name"))
	{
		bool bCyclic = !!pSettings->r_bool(sect_name, "cam_eff_cyclic");
		CAnimatorCamEffector* cam_anm = xr_new<CAnimatorCamEffector>( );
		cam_anm->SetType((ECamEffectorType) type);
		cam_anm->SetCyclic(bCyclic);
		pcstr fn = pSettings->r_string(sect_name, "cam_eff_name");
		cam_anm->Start(fn);
		A->Cameras( ).AddCamEffector(cam_anm);
	}
}

void AddEffector(CActor* A, s32 type, const shared_str& sect_name, CEffectorController* ec)
{
	if (pSettings->line_exist(sect_name, "pp_eff_name"))
	{
		bool bCyclic = !!pSettings->r_bool(sect_name, "pp_eff_cyclic");
		CPostprocessAnimatorControlled* pp_anm = xr_new<CPostprocessAnimatorControlled>(ec);
		pp_anm->SetType((EEffectorPPType) type);
		pp_anm->SetCyclic(bCyclic);
		pcstr fn = pSettings->r_string(sect_name, "pp_eff_name");
		pp_anm->Load(fn);
		A->Cameras( ).AddPPEffector(pp_anm);
	}

	if (pSettings->line_exist(sect_name, "cam_eff_name"))
	{
		bool bCyclic = !!pSettings->r_bool(sect_name, "cam_eff_cyclic");
		CCameraEffectorControlled* cam_anm = xr_new<CCameraEffectorControlled>(ec);
		cam_anm->SetType((ECamEffectorType) type);
		cam_anm->SetCyclic(bCyclic);
		pcstr fn = pSettings->r_string(sect_name, "cam_eff_name");
		cam_anm->Start(fn);
		A->Cameras( ).AddCamEffector(cam_anm);
	}
}

void AddEffector(CActor* A, s32 type, const shared_str& sect_name, GET_KOEFF_FUNC k_func)
{
	if (pSettings->line_exist(sect_name, "pp_eff_name"))
	{
		bool bCyclic = !!pSettings->r_bool(sect_name, "pp_eff_cyclic");
		CPostprocessAnimatorLerp* pp_anm = xr_new<CPostprocessAnimatorLerp>( );
		pp_anm->SetType((EEffectorPPType) type);
		pp_anm->SetCyclic(bCyclic);
		pcstr fn = pSettings->r_string(sect_name, "pp_eff_name");
		pp_anm->SetFactorFunc(k_func);
		pp_anm->Load(fn);
		A->Cameras( ).AddPPEffector(pp_anm);
	}

	if (pSettings->line_exist(sect_name, "cam_eff_name"))
	{
		bool bCyclic = !!pSettings->r_bool(sect_name, "cam_eff_cyclic");
		CAnimatorCamLerpEffector* cam_anm = xr_new<CAnimatorCamLerpEffector>( );
		cam_anm->SetFactorFunc(k_func);
		cam_anm->SetType((ECamEffectorType) type);
		cam_anm->SetCyclic(bCyclic);
		pcstr fn = pSettings->r_string(sect_name, "cam_eff_name");
		cam_anm->Start(fn);
		A->Cameras( ).AddCamEffector(cam_anm);
	}
}

void AddEffector(CActor* A, s32 type, const shared_str& sect_name, f32 factor)
{
	clamp(factor, 0.001f, 1.5f);
	if (pSettings->line_exist(sect_name, "pp_eff_name"))
	{
		bool bCyclic = !!pSettings->r_bool(sect_name, "pp_eff_cyclic");
		CPostprocessAnimatorLerpConst* pp_anm = xr_new<CPostprocessAnimatorLerpConst>( );
		pp_anm->SetType((EEffectorPPType) type);
		pp_anm->SetCyclic(bCyclic);
		pp_anm->SetPower(factor);
		pcstr fn = pSettings->r_string(sect_name, "pp_eff_name");
		pp_anm->Load(fn);
		A->Cameras( ).AddPPEffector(pp_anm);
	}

	if (pSettings->line_exist(sect_name, "cam_eff_name"))
	{
		bool bCyclic = !!pSettings->r_bool(sect_name, "cam_eff_cyclic");
		CAnimatorCamLerpEffectorConst* cam_anm = xr_new<CAnimatorCamLerpEffectorConst>( );
		cam_anm->SetFactor(factor);
		cam_anm->SetType((ECamEffectorType) type);
		cam_anm->SetCyclic(bCyclic);
		pcstr fn = pSettings->r_string(sect_name, "cam_eff_name");
		cam_anm->Start(fn);
		A->Cameras( ).AddCamEffector(cam_anm);
	}
}

void RemoveEffector(CActor* A, s32 type)
{
	A->Cameras( ).RemoveCamEffector((ECamEffectorType) type);
	A->Cameras( ).RemovePPEffector((EEffectorPPType) type);
}

CEffectorController::~CEffectorController( )
{
	R_ASSERT(!m_ce && !m_pe);
}

CAnimatorCamEffector::CAnimatorCamEffector( )
{
	m_bCyclic = true;
	m_objectAnimator = xr_new<CObjectAnimator>( );
	m_bAbsolutePositioning = false;
}

CAnimatorCamEffector::~CAnimatorCamEffector( )
{
	delete_data(m_objectAnimator);
}

void CAnimatorCamEffector::Start(pcstr fn)
{
	m_objectAnimator->Load(fn);
	m_objectAnimator->Play(Cyclic( ));
	fLifeTime = m_objectAnimator->GetLength( );
}

BOOL CAnimatorCamEffector::Valid( )
{
	if (Cyclic( ))
	{
		return TRUE;
	}

	return inherited::Valid( );
}

BOOL CAnimatorCamEffector::Process(fVector3& p, fVector3& d, fVector3& n, f32& fFov, f32& fFar, f32& fAspect)
{
	if (!inherited::Process(p, d, n, fFov, fFar, fAspect))
	{
		return FALSE;
	}

	const fMatrix4x4& m = m_objectAnimator->XFORM( );
	m_objectAnimator->Update(Device.fTimeDelta);

	if (!m_bAbsolutePositioning)
	{
		fMatrix4x4 Mdef;
		Mdef.identity( );
		Mdef.j = n;
		Mdef.k = d;
		Mdef.i.crossproduct(n, d);
		Mdef.c = p;

		fMatrix4x4 mr;
		mr.mul(Mdef, m);
		d = mr.k;
		n = mr.j;
		p = mr.c;
	}
	else
	{
		d = m.k;
		n = m.j;
		p = m.c;
	}

	return TRUE;
}

BOOL CAnimatorCamLerpEffector::Process(fVector3& p, fVector3& d, fVector3& n, f32& fFov, f32& fFar, f32& fAspect)
{
	if (!inherited::inherited::Process(p, d, n, fFov, fFar, fAspect))
	{
		return FALSE;
	}

	const fMatrix4x4& m = m_objectAnimator->XFORM( );
	m_objectAnimator->Update(Device.fTimeDelta);

	fMatrix4x4 Mdef;
	Mdef.identity( );
	Mdef.j = n;
	Mdef.k = d;
	Mdef.i.crossproduct(n, d);
	Mdef.c = p;

	fMatrix4x4 mr;
	mr.mul(Mdef, m);

	fQuaternion					q_src;
	fQuaternion					q_dst;
	fQuaternion					q_res;
	q_src.set(Mdef);
	q_dst.set(mr);

	f32	t = m_func( );
	clamp(t, 0.0f, 1.0f);

	VERIFY(t >= 0.0f && t <= 1.0f);
	q_res.slerp(q_src, q_dst, t);

	fMatrix4x4						res;
	res.rotation(q_res);
	res.c.lerp(p, mr.c, t);

	d = res.k;
	n = res.j;
	p = res.c;

	return TRUE;
}

CAnimatorCamLerpEffectorConst::CAnimatorCamLerpEffectorConst( ) : m_factor(0.0f)
{
	SetFactorFunc(GET_KOEFF_FUNC(this, &CAnimatorCamLerpEffectorConst::GetFactor));
}

CCameraEffectorControlled::CCameraEffectorControlled(CEffectorController* c) : m_controller(c)
{
	m_controller->SetCam(this);
	SetFactorFunc(GET_KOEFF_FUNC(m_controller, &CEffectorController::GetFactor));
}

CCameraEffectorControlled::~CCameraEffectorControlled( )
{
	m_controller->SetCam(NULL);
}

BOOL CCameraEffectorControlled::Valid( )
{
	return m_controller->Valid( );
}

#define SND_MIN_VOLUME_FACTOR (0.1f)

CSndShockEffector::CSndShockEffector( )
{
	m_snd_length = 0.0f;
	m_cur_length = 0.0f;
	m_stored_volume = -1.0f;
	m_actor = NULL;
}

CSndShockEffector::~CSndShockEffector( )
{
	psSoundVFactor = m_stored_volume;
	if (m_actor && (m_ce || m_pe))
	{
		RemoveEffector(m_actor, effHit);
	}

	R_ASSERT(!m_ce && !m_pe);
}

BOOL CSndShockEffector::Valid( )
{
	return (m_cur_length <= m_snd_length);
}

BOOL CSndShockEffector::InWork( )
{
	return inherited::Valid( );
}

f32 CSndShockEffector::GetFactor( )
{
	f32 f = (m_end_time - Device.fTimeGlobal) / m_life_time;

	f32 ff = f * m_life_time / 8.0f;
	return clampr(ff, 0.0f, 1.0f);
}

void CSndShockEffector::Start(CActor* A, f32 snd_length, f32 power)
{
	clamp(power, 0.1f, 1.5f);
	m_actor = A;
	m_snd_length = snd_length;

	if (m_stored_volume < 0.0f)
	{
		m_stored_volume = psSoundVFactor;
	}

	m_cur_length = 0.0f;
	psSoundVFactor = m_stored_volume * SND_MIN_VOLUME_FACTOR;

	static f32		xxx = 6.0f / 1.50f; //6sec on max power(1.5)

	m_life_time = power * xxx;
	m_end_time = Device.fTimeGlobal + m_life_time;

	AddEffector(A, effHit, "snd_shock_effector", this);
}

void CSndShockEffector::Update( )
{
	m_cur_length += Device.dwTimeDelta;
	f32 x = f32(m_cur_length) / m_snd_length;
	f32 y = 2.0f * x - 1;
	if (y > 0.0f)
	{
		psSoundVFactor = y * (m_stored_volume - m_stored_volume * SND_MIN_VOLUME_FACTOR) + m_stored_volume * SND_MIN_VOLUME_FACTOR;
	}
}

//////////////////////////////////////////////////////////////////////////

#define DELTA_ANGLE_X	0.5f * PI / 180
#define DELTA_ANGLE_Y	0.5f * PI / 180
#define DELTA_ANGLE_Z	0.5f * PI / 180
#define ANGLE_SPEED		1.5f	

CControllerPsyHitCamEffector::CControllerPsyHitCamEffector(ECamEffectorType type, const fVector3& src_pos, const fVector3& target_pos, f32 time) : inherited(eCEControllerPsyHit, flt_max)
{
	m_time_total = time;
	m_time_current = 0.0f;
	m_dangle_target.set(angle_normalize(Random.randFs(DELTA_ANGLE_X)), angle_normalize(Random.randFs(DELTA_ANGLE_Y)), angle_normalize(Random.randFs(DELTA_ANGLE_Z)));
	m_dangle_current.set(0.0f, 0.0f, 0.0f);
	m_position_source = src_pos;
	m_direction.sub(target_pos, src_pos);
	m_distance = m_direction.magnitude( );
	m_direction.normalize( );
}

const f32	_base_fov = 170.0f;
const f32	_max_fov_add = 160.0f;

BOOL CControllerPsyHitCamEffector::Process(fVector3& p, fVector3& d, fVector3& n, f32& fFov, f32& fFar, f32& fAspect)
{
	fMatrix4x4 Mdef;
	Mdef.identity( );
	Mdef.j.set(n);
	Mdef.k.set(m_direction);
	Mdef.i.crossproduct(n, m_direction);
	Mdef.c.set(p);

	//////////////////////////////////////////////////////////////////////////

	if (angle_lerp(m_dangle_current.x, m_dangle_target.x, ANGLE_SPEED, Device.fTimeDelta))
	{
		m_dangle_target.x = angle_normalize(Random.randFs(DELTA_ANGLE_X));
	}

	if (angle_lerp(m_dangle_current.y, m_dangle_target.y, ANGLE_SPEED, Device.fTimeDelta))
	{
		m_dangle_target.y = angle_normalize(Random.randFs(DELTA_ANGLE_Y));
	}

	if (angle_lerp(m_dangle_current.z, m_dangle_target.z, ANGLE_SPEED, Device.fTimeDelta))
	{
		m_dangle_target.z = angle_normalize(Random.randFs(DELTA_ANGLE_Z));
	}

	//////////////////////////////////////////////////////////////////////////

	if (m_time_current > m_time_total)
	{
		m_time_current = m_time_total;
	}

	f32 perc_past = m_time_current / m_time_total;
	f32 cur_dist = m_distance * perc_past;

	Mdef.c.mad(m_position_source, m_direction, cur_dist);
	fFov = _base_fov - _max_fov_add * perc_past;

	m_time_current += Device.fTimeDelta;

	//////////////////////////////////////////////////////////////////////////

	// Установить углы смещения
	fMatrix4x4 R;
	if (m_time_current > m_time_total)
	{
		R.identity( );
	}
	else
	{
		R.setHPB(m_dangle_current.x, m_dangle_current.y, m_dangle_current.z);
	}

	fMatrix4x4 mR;
	mR.mul(Mdef, R);

	d.set(mR.k);
	n.set(mR.j);
	p.set(mR.c);

	return TRUE;
}
