#include "stdafx.h"
#include "Actor.h"
#include "..\XR_3DA\CameraBase.h"
#ifdef DEBUG
#include "PHDebug.h"
#endif
#include "Hit.h"
#include "PHDestroyable.h"
#include "Car.h"

#include "Weapon.h"
#include "Inventory.h"

#include "SleepEffector.h"
#include "ActorEffector.h"
#include "level.h"
#include "..\XR_3DA\cl_intersect.h"
#include "gamemtllib.h"
#include "elevatorstate.h"
#include "CharacterPhysicsSupport.h"
#include "EffectorShot.h"
#include "phcollidevalidator.h"
#include "PHShell.h"

void CActor::cam_Set	(EActorCameras style)
{
	CCameraBase* old_cam = cam_Active();
	cam_active = style;
	old_cam->OnDeactivate();
	cam_Active()->OnActivate(old_cam);
}

f32 CActor::f_Ladder_cam_limit=1.0f;
void CActor::cam_SetLadder()
{
	CCameraBase* C			= cameras[eacFirstEye];
	g_LadderOrient			();
	f32 yaw				= (-XFORM().k.getH());
	f32& cam_yaw			= C->yaw;
	f32 delta_yaw			= angle_difference_signed(yaw,cam_yaw);

	if(-f_Ladder_cam_limit<delta_yaw&&f_Ladder_cam_limit>delta_yaw)
	{
		yaw					= cam_yaw+delta_yaw;
		f32 lo			= (yaw-f_Ladder_cam_limit);
		f32 hi			= (yaw+f_Ladder_cam_limit);
		C->lim_yaw[0]		= lo;
		C->lim_yaw[1]		= hi;
		C->bClampYaw		= true;
	}
}

void CActor::camUpdateLadder(f32 dt)
{
	if(!character_physics_support()->movement()->ElevatorState())
															return;
	if(cameras[eacFirstEye]->bClampYaw) return;
	f32 yaw				= (-XFORM().k.getH());

	f32& cam_yaw			= cameras[eacFirstEye]->yaw;
	f32 delta				= angle_difference_signed(yaw,cam_yaw);

	if(-0.05f<delta&&0.05f>delta)
	{
		yaw									= cam_yaw+delta;
		f32 lo							= (yaw-f_Ladder_cam_limit);
		f32 hi							= (yaw+f_Ladder_cam_limit);
		cameras[eacFirstEye]->lim_yaw[0]	= lo;
		cameras[eacFirstEye]->lim_yaw[1]	= hi;
		cameras[eacFirstEye]->bClampYaw		= true;
	}else{
		cam_yaw								+= delta * _min(dt*10.f,1.f) ;
	}

	CElevatorState* es = character_physics_support()->movement()->ElevatorState();
	if(es && es->State()==CElevatorState::clbClimbingDown)
	{
		f32& cam_pitch					= cameras[eacFirstEye]->pitch;
		const f32 ldown_pitch				= cameras[eacFirstEye]->lim_pitch.y;
		f32 delta							= angle_difference_signed(ldown_pitch,cam_pitch);
		if(delta>0.f)
			cam_pitch						+= delta* _min(dt*10.f,1.f) ;
	}
}

void CActor::cam_UnsetLadder()
{
	CCameraBase* C			= cameras[eacFirstEye];
	C->lim_yaw[0]			= 0;
	C->lim_yaw[1]			= 0;
	C->bClampYaw			= false;
}

f32 CActor::CameraHeight()
{
	fVector3						R;
	character_physics_support()->movement()->Box().getsize		(R);
	return						m_fCamHeightFactor*R.y;
}

IC f32 viewport_near(f32& w, f32& h)
{
	w = 2.f*VIEWPORT_NEAR*tan(deg2rad(Device.fFOV)/2.f);
	h = w*Device.fASPECT;
	f32	c	= _sqrt					(w*w + h*h);
	return	_max(_max(VIEWPORT_NEAR,_max(w,h)),c);
}

ICF void calc_point(fVector3& pt, f32 radius, f32 depth, f32 alpha)
{
	pt.x	= radius*_sin(alpha);
	pt.y	= radius+radius*_cos(alpha);
	pt.z	= depth;
}

ICF BOOL test_point(xrXRC& xrc, const fMatrix4x4& xform, const fMatrix3x3& mat, const fVector3& ext, f32 radius, f32 angle)
{
	fVector3				pt;
	calc_point			(pt,radius,VIEWPORT_NEAR/2,angle);
	xform.transform_tiny(pt);

	CDB::RESULT* it	=xrc.r_begin();
	CDB::RESULT* end=xrc.r_end	();
	for (; it!=end; it++)	{
		CDB::RESULT&	O	= *it;
		if (GMLib.GetMaterialByIdx(O.material)->Flags.is(SGameMtl::flPassable)) continue;
		if (CDB::TestBBoxTri(mat,pt,ext,O.verts,FALSE))
			return		TRUE;
	}
	return FALSE;
}

#include "physics.h"
#include "PHActivationShape.h"
#include "DebugRenderer.h"
void CActor::cam_Update(f32 dt, f32 fFOV)
{
	if(m_holder)		return;

	if(mstate_real & mcClimb&&cam_active!=eacFreeLook)
		camUpdateLadder(dt);

	fVector3 point = { 0.0f,CameraHeight( ),0.0f };
	fVector3 dangle = { 0.0f,0.0f,0.0f };

	fMatrix4x4			xform;
	fMatrix4x4			xformR;
	xform.setXYZ		(0,r_torso.yaw,0);
	xform.translate_over(XFORM().c);

	// lookout
	if (this == Level().CurrentControlEntity())
	{
		if (!fis_zero(r_torso_tgt_roll)){
			fVector3 src_pt;
			fVector3 tgt_pt;
			f32 radius		= point.y*0.5f;
			f32 alpha			= r_torso_tgt_roll/2.f;
			f32 dZ			= ((PI_DIV_2-((PI+alpha)/2)));
			calc_point			(tgt_pt,radius,0,alpha);
			src_pt.set			(0,tgt_pt.y,0);
			// init valid angle
			f32 valid_angle	= alpha;
			// xform with roll
			xformR.setXYZ		(-r_torso.pitch,r_torso.yaw,-dZ);
			fMatrix3x3			mat;
			mat.i				= xformR.i;
			mat.j				= xformR.j;
			mat.k				= xformR.k;
			// get viewport params
			f32 w;
			f32 h;
			f32 c				= viewport_near(w,h);
			w/=2.f;
			h/=2.f;
			// find tris
			fBox3 box;
			box.invalidate		();
			box.modify			(src_pt);
			box.modify			(tgt_pt);
			box.grow			(c);

			// query
			fVector3			bc;
			fVector3			bd;
			fBox3				xf			;
			xf.xform			(box,xform)	;
			xf.get_CD			(bc,bd)		;

			xrXRC				xrc			;
			xrc.box_options		(0)			;
			xrc.box_query		(Level().ObjectSpace.GetStaticModel(), bc, bd)		;
			u32 tri_count		= xrc.r_count();
			if (tri_count)		{
				f32 da		= 0.f;
				BOOL bIntersect	= FALSE;
				fVector3	ext		= {w,h,VIEWPORT_NEAR/2};
				if (test_point(xrc,xform,mat,ext,radius,alpha)){
					da			= PI/1000.f;
					if (!fis_zero(r_torso.roll))
						da		*= r_torso.roll/_abs(r_torso.roll);
					for (f32 angle=0.f; _abs(angle)<_abs(alpha); angle+=da)
						if (test_point(xrc,xform,mat,ext,radius,angle)) { bIntersect=TRUE; break; } 
						valid_angle	= bIntersect?angle:alpha;
				} 
			}
			r_torso.roll		= valid_angle*2.f;
			r_torso_tgt_roll	= r_torso.roll;
		}
		else
		{	
			r_torso_tgt_roll = 0.f;
			r_torso.roll = 0.f;
		}
	}

	if (!fis_zero(r_torso.roll))
	{
		f32 radius		= point.y*0.5f;
		f32 valid_angle	= r_torso.roll/2.f;
		calc_point			(point,radius,0,valid_angle);
		dangle.z			= (PI_DIV_2-((PI+valid_angle)/2));
	}

	f32 flCurrentPlayerY	= xform.c.y;

	// Smooth out stair step ups
	if ((character_physics_support()->movement()->Environment()==peOnGround) && (flCurrentPlayerY-fPrevCamPos>0)){
		fPrevCamPos			+= dt*1.5f;
		if (fPrevCamPos > flCurrentPlayerY)
			fPrevCamPos		= flCurrentPlayerY;
		if (flCurrentPlayerY-fPrevCamPos>0.2f)
			fPrevCamPos		= flCurrentPlayerY-0.2f;
		point.y				+= fPrevCamPos-flCurrentPlayerY;
	}else{
		fPrevCamPos			= flCurrentPlayerY;
	}
	f32 _viewport_near			= VIEWPORT_NEAR;
	// calc point
	xform.transform_tiny			(point);

	CCameraBase* C					= cam_Active();

	if(eacFirstEye == cam_active)
	{
//		CCameraBase* C				= cameras[eacFirstEye];
	
		xrXRC						xrc			;
		xrc.box_options				(0)			;
		xrc.box_query				(Level().ObjectSpace.GetStaticModel(), point, fVector3().set(VIEWPORT_NEAR,VIEWPORT_NEAR,VIEWPORT_NEAR) );
		u32 tri_count				= xrc.r_count();
		if (tri_count)
		{
			_viewport_near			= 0.01f;
		}
		else
		{
			xr_vector<ISpatial*> ISpatialResult;
			g_SpatialSpacePhysic->q_box(ISpatialResult, 0, STYPE_PHYSIC, point, fVector3().set(VIEWPORT_NEAR,VIEWPORT_NEAR,VIEWPORT_NEAR));
			for (u32 o_it=0; o_it<ISpatialResult.size(); o_it++)
			{
				CPHShell*		pCPHS= smart_cast<CPHShell*>(ISpatialResult[o_it]);
				if (pCPHS)
				{
					_viewport_near			= 0.01f;
					break;
				}
			}
		}
	}
/*
	{
		CCameraBase* C				= cameras[eacFirstEye];
		f32 oobox_size			= 2*VIEWPORT_NEAR;


		fMatrix4x4						_rot;
		_rot.k						= C->vDirection;
		_rot.c						= C->vPosition;
		_rot.i.crossproduct			(C->vNormal,	_rot.k);
		_rot.j.crossproduct			(_rot.k,		_rot.i);

		
		fVector3						vbox; 
		vbox.set					(oobox_size, oobox_size, oobox_size);


		Level().debug_renderer().draw_aabb  (C->vPosition, 0.05f, 0.051f, 0.05f, D3DCOLOR_XRGB(0,255,0));
		Level().debug_renderer().draw_obb  (_rot, fVector3().div(vbox,2.0f), D3DCOLOR_XRGB(255,0,0));

		dMatrix3					d_rot;
		PHDynamicData::FMXtoDMX		(_rot, d_rot);

		CPHActivationShape			activation_shape;
		activation_shape.Create		(point, vbox, this);

		dBodySetRotation			(activation_shape.ODEBody(), d_rot);

		CPHCollideValidator::SetDynamicNotCollide(activation_shape);
		activation_shape.Activate	(vbox,1,1.f,0.0F);

		point.set					(activation_shape.Position());
		
		activation_shape.Destroy	();
	}
*/
	C->Update						(point,dangle);
	C->f_fov						= fFOV;
	if(eacFirstEye != cam_active)
	{
		cameras[eacFirstEye]->Update	(point,dangle);
		cameras[eacFirstEye]->f_fov		= fFOV;
	}
	
	if( psActorFlags.test(AF_PSP) )
	{
		Cameras().Update			(C);
	}
	else
	{
		Cameras().Update			(cameras[eacFirstEye]);
	}

	fCurAVelocity			= vPrevCamDir.sub(cameras[eacFirstEye]->vDirection).magnitude()/Device.fTimeDelta;
	vPrevCamDir				= cameras[eacFirstEye]->vDirection;

	if (Level().CurrentEntity() == this)
	{
		Level().Cameras().Update	(C);
		if(eacFirstEye == cam_active && !Level().Cameras().GetCamEffector(cefDemo)){
			Cameras().ApplyDevice	(_viewport_near);
		}
	}
}

// shot effector stuff
void CActor::update_camera (CCameraShotEffector* effector)
{
	if (!effector) return;
	//	if (Level().CurrentViewEntity() != this) return;

	CCameraBase* pACam = cam_FirstEye();
	if (!pACam) return;

	if (pACam->bClampPitch)
	{
		while (pACam->pitch < pACam->lim_pitch[0])
			pACam->pitch += PI_MUL_2;
		while (pACam->pitch > pACam->lim_pitch[1])
			pACam->pitch -= PI_MUL_2;
	}

	effector->ApplyLastAngles(&(pACam->pitch), &(pACam->yaw));

	if (pACam->bClampYaw)
	{
		clamp(pACam->yaw, pACam->lim_yaw[0], pACam->lim_yaw[1]);
	}

	if (pACam->bClampPitch)
	{
		clamp(pACam->pitch, pACam->lim_pitch[0], pACam->lim_pitch[1]);
	}
}

#ifdef DEBUG
void dbg_draw_frustum (f32 FOV, f32 _FAR, f32 A, fVector3& P, fVector3& D, fVector3& U);
extern	flags32	dbg_net_Draw_Flags;

void CActor::OnRender	()
{
	if (!bDebug)				return;

	if ((dbg_net_Draw_Flags.is_any((1<<5))))
		character_physics_support()->movement()->dbg_Draw	();

	OnRender_Network();

	inherited::OnRender();
}
#endif

void CActor::LoadSleepEffector	(pcstr section)
{
	if(!m_pSleepEffector) 
		m_pSleepEffector = xr_new<SSleepEffector>();

	m_pSleepEffector->ppi.duality.h			= pSettings->r_float(section,"duality_h");
	m_pSleepEffector->ppi.duality.v			= pSettings->r_float(section,"duality_v");
	m_pSleepEffector->ppi.gray				= pSettings->r_float(section,"gray");
	m_pSleepEffector->ppi.blur				= pSettings->r_float(section,"blur");
	m_pSleepEffector->ppi.noise.intensity	= pSettings->r_float(section,"noise_intensity");
	m_pSleepEffector->ppi.noise.grain		= pSettings->r_float(section,"noise_grain");
	m_pSleepEffector->ppi.noise.fps			= pSettings->r_float(section,"noise_fps");
	VERIFY(!fis_zero(m_pSleepEffector->ppi.noise.fps));

	sscanf(pSettings->r_string(section,"color_base"),	"%f,%f,%f", &m_pSleepEffector->ppi.color_base.r, &m_pSleepEffector->ppi.color_base.g, &m_pSleepEffector->ppi.color_base.b);
	sscanf(pSettings->r_string(section,"color_gray"),	"%f,%f,%f", &m_pSleepEffector->ppi.color_gray.r, &m_pSleepEffector->ppi.color_gray.g, &m_pSleepEffector->ppi.color_gray.b);
	sscanf(pSettings->r_string(section,"color_add"),	"%f,%f,%f", &m_pSleepEffector->ppi.color_add.r,  &m_pSleepEffector->ppi.color_add.g,  &m_pSleepEffector->ppi.color_add.b);

	m_pSleepEffector->time				= pSettings->r_float(section,"time");
	m_pSleepEffector->time_attack		= pSettings->r_float(section,"time_attack");
	m_pSleepEffector->time_release		= pSettings->r_float(section,"time_release");
}
