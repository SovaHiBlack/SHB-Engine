#include "stdafx.h"

#ifdef DEBUG
#include "ode_include.h"
#include "..\XR_3DA\StatGraph.h"
#include "PHDebug.h"
#include "phworld.h"
#endif
#include "alife_space.h"
#include "Hit.h"
#include "PHDestroyable.h"
#include "car.h"
#include "Actor.h"
#include "CameraLook.h"
#include "CameraFirstEye.h"
#include "level.h"
#include "..\XR_3DA\cameramanager.h"

bool CCar::HUDView() const		
{
	return active_camera->tag==ectFirst;
}

void	CCar::cam_Update			(f32 dt, f32 fov)
{
	VERIFY(!ph_world->Processing());
	fVector3							P;
	fVector3						Da;
	Da.set							(0.0f,0.0f,0.0f);
	//bool							owner = !!Owner();

	XFORM().transform_tiny			(P,m_camera_position);

	switch(active_camera->tag) {
	case ectFirst:
		// rotate head
		if(OwnerActor()) OwnerActor()->Orientation().yaw	= -active_camera->yaw;
		if(OwnerActor()) OwnerActor()->Orientation().pitch	= -active_camera->pitch;
		break;
	case ectChase:					break;
	case ectFree:					break;
	}
	active_camera->f_fov			= fov;
	active_camera->Update			(P,Da);
	Level().Cameras().Update		(active_camera);
}

void	CCar::OnCameraChange		(s32 type)
{
	if(Owner())
	{
		if	(type==ectFirst)
		{
			Owner()->setVisible(FALSE);
		}
		else if(active_camera->tag==ectFirst)
		{
			Owner()->setVisible(TRUE);
		}
	}
	
	if (!active_camera||active_camera->tag!=type){
		active_camera	= camera[type];
		if (ectFree==type){
			fVector3 xyz;
			XFORM().getXYZi(xyz);
			active_camera->yaw		= xyz.y;
		}
	}
}
