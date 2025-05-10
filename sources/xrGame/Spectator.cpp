// Actor.cpp: implementation of the CSpectator class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "spectator.h"
#include "effectorfall.h"
#include "CameraLook.h"
#include "CameraFirstEye.h"
#include "actor.h"
#include "hudmanager.h"
#include "xrServer_Objects.h"
#include "game_cl_base.h"
#include "level.h"
#include "xr_level_controller.h"
#include "seniority_hierarchy_holder.h"
#include "team_hierarchy_holder.h"
#include "squad_hierarchy_holder.h"
#include "group_hierarchy_holder.h"
#include "..\XR_3DA\CameraManager.h"
#include "Inventory.h"
#include "huditem.h"
#include "clsid_game.h"
#include "string_table.h"
#include "map_manager.h"
#include "..\XR_3DA\IGame_Persistent.h"

//--------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CSpectator::CSpectator( ) : CGameObject( )
{
	// Cameras
	cameras[eacFirstEye] = xr_new<CCameraFirstEye>(this);
	cameras[eacFirstEye]->Load("actor_firsteye_cam");

	cameras[eacLookAt] = xr_new<CCameraLook>(this);
	cameras[eacLookAt]->Load("actor_look_cam");

	cameras[eacFreeLook] = xr_new<CCameraLook>(this);
	cameras[eacFreeLook]->Load("actor_free_cam");

	cameras[eacFreeFly] = xr_new<CCameraFirstEye>(this);
	cameras[eacFreeFly]->Load("actor_firsteye_cam");

//	cam_active				= eacFreeFly;
	cam_active = eacFreeLook;
	look_idx = 0;
	m_pActorToLookAt = NULL;
}

CSpectator::~CSpectator( )
{
	for (s32 i = 0; i < eacMaxCam; ++i)
	{
		xr_delete(cameras[i]);
	}
}

void CSpectator::UpdateCL( )
{
	inherited::UpdateCL( );

	if (g_pGameLevel->CurrentViewEntity( ) == this)
	{
		if (eacFreeFly != cam_active)
		{
//-------------------------------------

//-------------------------------------
			s32 idx = 0;
			game_PlayerState* P = Game( ).local_player;
			if (P && (P->team >= 0) && (P->team < (s32)Level( ).seniority_holder( ).teams( ).size( )))
			{
				const CTeamHierarchyHolder& T = Level( ).seniority_holder( ).team(P->team);
				for (u32 i = 0; i < T.squads( ).size( ); ++i)
				{
					const CSquadHierarchyHolder& S = T.squad(i);
					for (u32 j = 0; j < S.groups( ).size( ); ++j)
					{
						const CGroupHierarchyHolder& G = S.group(j);
						for (u32 k = 0; k < G.members( ).size( ); ++k)
						{
							CActor* A = smart_cast<CActor*>(G.members( )[k]);
							if (A/*&&A->g_Alive()*/)
							{
								if (idx == look_idx)
								{
									cam_Update(A);
									return;
								}
								++idx;
							}
						}
					}
				}
			}
			// не найден объект с таким индексом - сбросим на первый объект
			look_idx = 0;
			// никого нет за кем смотреть - переключимся на 
			if (0 == idx)
			{
				cam_Set(eacFreeFly);
			}
		}
		// по умолчанию eacFreeFly
		cam_Update(0);
	}
}

void CSpectator::shedule_Update(u32 DT)
{
	inherited::shedule_Update(DT);
//	if (!getEnabled())	return;
	if (!Ready( ))
	{
		return;
	}
}

#define START_ACCEL		16.0f
static f32 Accel_mul = START_ACCEL;

void CSpectator::IR_OnKeyboardPress(s32 cmd)
{
	if (Remote( ))
	{
		return;
	}

	switch (cmd)
	{
		case kACCEL:
		{
			Accel_mul = START_ACCEL * 2;
		}
		break;
		case kCAM_1:
		{
			if (m_pActorToLookAt)
			{
				cam_Set(eacFirstEye);
			}
			else
			{
				cam_Set(eacFreeFly);
			}
		}
		break;
		case kCAM_2:
		{
		}
		break;
		case kCAM_3:
		{
		}
		break;
		case kCAM_4:
		{
			cam_Set(eacFreeFly);
			m_pActorToLookAt = NULL;
		}
		break;
		case kWPN_FIRE:
		{
			if (cam_active != eacFreeFly)
			{
				++look_idx;
				if (cam_active == eacFirstEye && m_pActorToLookAt)
				{
					FirstEye_ToPlayer(m_pActorToLookAt);
				}
			}
		}
		break;
		case kWPN_ZOOM:
		{
		}
		break;
	}
}

void CSpectator::IR_OnKeyboardRelease(s32 cmd)
{
	switch (cmd)
	{
		case kACCEL:
		{
			Accel_mul = START_ACCEL;
		}
		break;
	}
}

void CSpectator::IR_OnKeyboardHold(s32 cmd)
{
	if (Remote( ))
	{
		return;
	}

	game_PlayerState* PS = Game( ).local_player;

	if ((cam_active == eacFreeFly) || (cam_active == eacFreeLook))
	{
		CCameraBase* C = cameras[cam_active];
		fVector3 vmove = {0.0f,0.0f,0.0f};
		switch (cmd)
		{
			case kUP:
			case kDOWN:
			case kCAM_ZOOM_IN:
			case kCAM_ZOOM_OUT:
			{
				cameras[cam_active]->Move(cmd);
			}
			break;
			case kLEFT:
			case kRIGHT:
			{
				if (eacFreeLook != cam_active)
				{
					cameras[cam_active]->Move(cmd);
				}
			}
			break;
			case kFWD:
			{
				vmove.mad(C->vDirection, Device.fTimeDelta * Accel_mul);
			}
				break;
			case kBACK:
			{
				vmove.mad(C->vDirection, -Device.fTimeDelta * Accel_mul);
			}
				break;
			case kR_STRAFE:
			{
				fVector3 right;
				right.crossproduct(C->vNormal, C->vDirection);
				vmove.mad(right, Device.fTimeDelta * Accel_mul);
			}
			break;
			case kL_STRAFE:
			{
				fVector3 right;
				right.crossproduct(C->vNormal, C->vDirection);
				vmove.mad(right, -Device.fTimeDelta * Accel_mul);
			}
			break;
		}

		if (cam_active != eacFreeFly)
		{
			XFORM( ).c.add(vmove);
		}
	}
}

void CSpectator::IR_OnMouseMove(s32 dx, s32 dy)
{
	if (Remote( ))
	{
		return;
	}

	CCameraBase* C = cameras[cam_active];
	f32 scale = (C->f_fov / g_fov) * psMouseSens * psMouseSensScale / 50.0f;
	if (dx)
	{
		f32 d = f32(dx) * scale;
		cameras[cam_active]->Move((d < 0) ? kLEFT : kRIGHT, _abs(d));
	}

	if (dy)
	{
		f32 d = ((psMouseInvert.test(1)) ? -1 : 1) * f32(dy) * scale * 3.0f / 4.0f;
		cameras[cam_active]->Move((d > 0) ? kUP : kDOWN, _abs(d));
	}
}

void CSpectator::FirstEye_ToPlayer(CObject* pObject)
{
	CObject* pCurViewEntity = Level( ).CurrentEntity( );
	if (pCurViewEntity)
	{
		CActor* pOldActor = smart_cast<CActor*>(pCurViewEntity);
		if (pOldActor)
		{
			pOldActor->inventory( ).Items_SetCurrentEntityHud(false);
		}

		if (pCurViewEntity->CLS_ID != CLSID_SPECTATOR)
		{
			Engine.Sheduler.Unregister(pCurViewEntity);
			Engine.Sheduler.Register(pCurViewEntity, TRUE);
		}
	}

	if (pObject)
	{
		Level( ).SetEntity(pObject);

		Engine.Sheduler.Unregister(pObject);
		Engine.Sheduler.Register(pObject, TRUE);

		CActor* pActor = smart_cast<CActor*> (pObject);
		if (pActor)
		{
			pActor->inventory( ).Items_SetCurrentEntityHud(true);

			CHudItem* pHudItem = smart_cast<CHudItem*>(pActor->inventory( ).ActiveItem( ));
			if (pHudItem)
			{
				pHudItem->OnStateSwitch(pHudItem->GetState( ));
			}
		}
	}
}

void CSpectator::cam_Set(EActorCameras style)
{
	CCameraBase* old_cam = cameras[cam_active];
	//-----------------------------------------------
	if (style == eacFirstEye)
	{
		FirstEye_ToPlayer(m_pActorToLookAt);
	}

	if (cam_active == eacFirstEye)
	{
		FirstEye_ToPlayer(this);
	}
	//-----------------------------------------------
	cam_active = style;
	old_cam->OnDeactivate( );
	cameras[cam_active]->OnActivate(old_cam);
}

void CSpectator::cam_Update(CActor* A)
{
	if (A)
	{
		const fMatrix4x4& M = A->XFORM( );
		CCameraBase* pACam = A->cam_Active( );
		CCameraBase* cam = cameras[cam_active];
		switch (cam_active)
		{
			case eacFirstEye:
			{
				fVector3 P;
				fVector3 D;
				fVector3 N;
				pACam->Get(P, D, N);
				cam->Set(P, D, N);
			}
			break;
			case eacLookAt:
			{
				f32 y;
				f32 p;
				f32 r;
				M.getHPB(y, p, r);
				cam->Set(pACam->yaw, pACam->pitch, -r);
			}
			case eacFreeLook:
			{
				cam->SetParent(A);
				fMatrix4x4 tmp;
				tmp.identity( );

				fVector3 point;
				fVector3 point1;
				fVector3 dangle;
				point.set(0.0f, 1.6f, 0.0f);
				point1.set(0.0f, 1.6f, 0.0f);
				M.transform_tiny(point);
				tmp.translate_over(point);
				tmp.transform_tiny(point1);
				if (!A->g_Alive( ))
				{
					point.set(point1);
				}

				cam->Update(point, dangle);
			}
			break;
		}
		//-----------------------------------
		fVector3 P;
		fVector3 D;
		fVector3 N;
		cam->Get(P, D, N);
		cameras[eacFreeFly]->Set(P, D, N);
		cameras[eacFreeFly]->Set(cam->yaw, cam->pitch, 0);
		P.y -= 1.6f;
		XFORM( ).translate_over(P);
		//-----------------------------------
		g_pGameLevel->Cameras( ).Update(cam);
	}
	else
	{
		CCameraBase* cam = cameras[eacFreeFly];
		fVector3 point;
		fVector3 dangle;
		point.set(0.0f, 1.6f, 0.0f);
		XFORM( ).transform_tiny(point);

		// apply shift
		dangle.set(0.0f, 0.0f, 0.0f);

		cam->Update(point, dangle);
//		cam->vPosition.set(point0);
		g_pGameLevel->Cameras( ).Update(cam);
		// hud output
	}
}

BOOL CSpectator::net_Spawn(CSE_Abstract* DC)
{
	BOOL res = inherited::net_Spawn(DC);
	if (!res)
	{
		return FALSE;
	}

	CSE_Abstract* E = (CSE_Abstract*)(DC);
	if (!E)
	{
		return FALSE;
	}

	cam_active = eacFreeFly;
	look_idx = 0;

	cameras[cam_active]->Set(-E->o_Angle.y, -E->o_Angle.x, 0);		// set's camera orientation
	cameras[cam_active]->vPosition.set(E->o_Position);

	if (OnServer( ))
	{
		E->s_flags.set(M_SPAWN_OBJECT_LOCAL, TRUE);
	}

	return TRUE;
}

void			CSpectator::net_Destroy( )
{
	inherited::net_Destroy( );
	Level( ).MapManager( ).RemoveMapLocationByObjectID(ID( ));
}

void			CSpectator::net_Relcase(CObject* O)
{
	if (O != m_pActorToLookAt)
	{
		return;
	}

	m_pActorToLookAt = NULL;
	if (!m_pActorToLookAt)
	{
		cam_Set(eacFreeFly);
	}
}
