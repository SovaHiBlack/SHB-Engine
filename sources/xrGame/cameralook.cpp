#include "stdafx.h"

#include "CameraLook.h"
#include "..\XR_3DA\Cameramanager.h"
#include "xr_level_controller.h"
#include "actor.h"

CCameraLook::CCameraLook(CObject* p, u32 flags ) 
:CCameraBase(p, flags)
{
}

void CCameraLook::Load(pcstr section)
{
	inherited::Load		(section);
	style				= csLookAt;
	lim_zoom			= pSettings->r_fvector2	(section,"lim_zoom");
	dist				= (lim_zoom[0]+lim_zoom[1])*0.5f;
	prev_d				= 0;
}

CCameraLook::~CCameraLook()
{
}

void CCameraLook::Update(fVector3& point, fVector3& /**noise_dangle/**/)
{
	vPosition.set		(point);
	fMatrix4x4 mR;
	mR.setHPB			(-yaw,-pitch,-roll);

	vDirection.set		(mR.k);
	vNormal.set			(mR.j);

	if (m_Flags.is(flRelativeLink)){
		parent->XFORM().transform_dir(vDirection);
		parent->XFORM().transform_dir(vNormal);
	}

	fVector3				vDir;
	collide::rq_result	R;

	f32				covariance = VIEWPORT_NEAR*6.0f;
	vDir.invert			(vDirection);
	g_pGameLevel->ObjectSpace.RayPick( point, vDir, dist+covariance, collide::rqtBoth, R, parent);

	f32 d				= psCamSlideInert*prev_d+(1.f-psCamSlideInert)*(R.range-covariance);
	prev_d = d;
	
	vPosition.mul		(vDirection,-d-VIEWPORT_NEAR);
	vPosition.add		(point);
}

void CCameraLook::Move( int cmd, f32 val, f32 factor)
{
	switch (cmd){
	case kCAM_ZOOM_IN:	dist	-= val?val:(rot_speed.z*Device.fTimeDelta);	break;
	case kCAM_ZOOM_OUT:	dist	+= val?val:(rot_speed.z*Device.fTimeDelta);	break;
	case kDOWN:			pitch	-= val?val:(rot_speed.x*Device.fTimeDelta/factor);	break;
	case kUP:			pitch	+= val?val:(rot_speed.x*Device.fTimeDelta/factor);	break;
	case kLEFT:			yaw		-= val?val:(rot_speed.y*Device.fTimeDelta/factor);	break;
	case kRIGHT:		yaw		+= val?val:(rot_speed.y*Device.fTimeDelta/factor);	break;
	}
	if (bClampYaw)		clamp(yaw,lim_yaw[0],lim_yaw[1]);
	if (bClampPitch)	clamp(pitch,lim_pitch[0],lim_pitch[1]);
	clamp			(dist,lim_zoom[0],lim_zoom[1]);	
}

void CCameraLook::OnActivate( CCameraBase* old_cam )
{
	if (old_cam&&(m_Flags.is(flRelativeLink)==old_cam->m_Flags.is(flRelativeLink)))
	{
		yaw				= old_cam->yaw;
		vPosition.set	(old_cam->vPosition);
	}
	if (yaw>PI_MUL_2) yaw-=PI_MUL_2;
	if (yaw<-PI_MUL_2)yaw+=PI_MUL_2;
}

#include "..\XR_3DA\xr_input.h"
#include "visual_memory_manager.h"
#include "actor_memory.h"

int cam_dik = DIK_LSHIFT;

fVector3 CCameraLook2::m_cam_offset;
void CCameraLook2::OnActivate( CCameraBase* old_cam )
{
	CCameraLook::OnActivate( old_cam );
	for(int i=0; i < 2048; ++i)
	{
		if(is_binded(kEXT_1, i) )
		{
			cam_dik = i;
			break;
		}
	}
}

void CCameraLook2::Update(fVector3& point, fVector3&)
{
	if(!m_locked_enemy)
	{//autoaim
		if( pInput->iGetAsyncKeyState(cam_dik) )
		{
			const CVisualMemoryManager::VISIBLES& vVisibles = Actor()->memory().visual().objects();
			CVisualMemoryManager::VISIBLES::const_iterator v_it = vVisibles.begin();
			f32 _nearest_dst	= flt_max;

			for (; v_it!=vVisibles.end(); ++v_it)
			{
				const CObject*	_object_			= (*v_it).m_object;
				if (!Actor()->memory().visual().visible_now(smart_cast<const CGameObject*>(_object_)))	continue;

				CObject* object_ = const_cast<CObject*>(_object_);
				

				CEntityAlive*	EA					= smart_cast<CEntityAlive*>(object_);
				if(!EA || !EA->g_Alive())			continue;
				
				f32 d = object_->Position().distance_to_xz(Actor()->Position());
				if( !m_locked_enemy || d<_nearest_dst)
				{
					m_locked_enemy	= object_;
					_nearest_dst	= d;
				}
			}
//.			if(m_locked_enemy) Msg("enemy is %s", *m_locked_enemy->cNameSect() );
		}
	}else
	{
		if( !pInput->iGetAsyncKeyState(cam_dik) ){
			m_locked_enemy	= NULL;
//.			Msg				("enemy is NILL");
		}
	}

	if(m_locked_enemy)
		UpdateAutoAim	();


	fMatrix4x4 mR;
	mR.setHPB						(-yaw,-pitch,-roll);

	vDirection.set					(mR.k);
	vNormal.set						(mR.j);

	fMatrix4x4							a_xform;
	a_xform.setXYZ					(0, -yaw, 0);
	a_xform.translate_over			(point);
	fVector3 _off					= m_cam_offset;
	a_xform.transform_tiny			(_off);
	vPosition.set					(_off);
}

void CCameraLook2::UpdateAutoAim()
{
	fVector3								_dest_point;
	m_locked_enemy->Center				(_dest_point);
	_dest_point.y						+= 0.2f;

	fVector3								_dest_dir;
	_dest_dir.sub						(_dest_point, vPosition);
	
	fMatrix4x4								_m;
	_m.identity							();
	_m.k.normalize_safe					(_dest_dir);
	fVector3::generate_orthonormal_basis	(_m.k, _m.j, _m.i);

	fVector3								xyz;
	_m.getXYZi							(xyz);

	yaw				= angle_inertion_var(	yaw,xyz.y,
											m_autoaim_inertion_yaw.x,
											m_autoaim_inertion_yaw.y,
											PI,
											Device.fTimeDelta);

	pitch			= angle_inertion_var(	pitch,xyz.x,
											m_autoaim_inertion_pitch.x,
											m_autoaim_inertion_pitch.y,
											PI,
											Device.fTimeDelta);
}

void CCameraLook2::Load(pcstr section)
{
	CCameraLook::Load		(section);
	m_cam_offset			= pSettings->r_fvector3	(section,"offset");
	m_autoaim_inertion_yaw	= pSettings->r_fvector2	(section,"autoaim_speed_y");
	m_autoaim_inertion_pitch= pSettings->r_fvector2	(section,"autoaim_speed_x");
}
