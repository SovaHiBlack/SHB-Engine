#include "stdafx.h"

#include "Actor.h"
#include "ActorMemory.h"
//#include "Actor_flags.h"
#include "Inventory.h"
#include "xrserver_objects_alife_monsters.h"
#include "xrServer.h"

#include "CameraLook.h"
#include "CameraFirstEye.h"

#include "ActorEffector.h"

#include "PHWorld.h"
#include "level.h"
#include "xr_level_controller.h"
#include "game_cl_base.h"
#include "InfoPortion.h"
#include "alife_registry_wrappers.h"
#include "..\XR_3DA\skeletonanimated.h"
#include "ClientSpawnManager.h"
#include "Hit.h"
#include "PHDestroyable.h"
#include "CharacterPhysicsSupport.h"
#include "Grenade.h"
#include "WeaponMagazined.h"
#include "CustomOutfit.h"

#include "actor_anim_defs.h"

#include "MapManager.h"
#include "HUDManager.h"
#include "ui/UIArtefactPanel.h"//
#include "ui/UIMainIngameWnd.h"//
#include "gamepersistent.h"
#include "GameObject_space.h"
#include "GameTaskManager.h"
#include "game_base_kill_type.h"
#include "HolderCustom.h"
#include "ActorStatisticManager.h"
#include "game_cl_base_weapon_usage_statistic.h"
#include "clsid_game.h"

#ifdef DEBUG
#	include "DebugRenderer.h"
#endif

s32			g_cl_InterpolationType = 0;
u32			g_cl_InterpolationMaxPoints = 0;
s32			g_dwInputUpdateDelta = 20;
BOOL		net_cl_inputguaranteed = FALSE;
CActor* g_actor = NULL;

CActor* Actor( )
{
	VERIFY(g_actor);
	return g_actor;
}

//--------------------------------------------------------------------
void	CActor::ConvState(u32 mstate_rl, string128* buf)
{
	strcpy(*buf, "");
	if (isActorAccelerated(mstate_rl, IsZoomAimingMode( )))
	{
		strcat(*buf, "Accel ");
	}

	if (mstate_rl & mcCrouch)
	{
		strcat(*buf, "Crouch ");
	}

	if (mstate_rl & mcFwd)
	{
		strcat(*buf, "Fwd ");
	}

	if (mstate_rl & mcBack)
	{
		strcat(*buf, "Back ");
	}

	if (mstate_rl & mcLStrafe)
	{
		strcat(*buf, "LStrafe ");
	}

	if (mstate_rl & mcRStrafe)
	{
		strcat(*buf, "RStrafe ");
	}

	if (mstate_rl & mcJump)
	{
		strcat(*buf, "Jump ");
	}

	if (mstate_rl & mcFall)
	{
		strcat(*buf, "Fall ");
	}

	if (mstate_rl & mcTurn)
	{
		strcat(*buf, "Turn ");
	}

	if (mstate_rl & mcLanding)
	{
		strcat(*buf, "Landing ");
	}

	if (mstate_rl & mcLLookout)
	{
		strcat(*buf, "LLookout ");
	}

	if (mstate_rl & mcRLookout)
	{
		strcat(*buf, "RLookout ");
	}

	if (m_bJumpKeyPressed)
	{
		strcat(*buf, "+Jumping ");
	}
}
//--------------------------------------------------------------------
void CActor::net_Export(CNetPacket& P)					// export to server
{
	//CSE_ALifeCreatureAbstract
	u8					flags = 0;
	P.w_float(GetfHealth( ));
	P.w_u32(Level( ).timeServer( ));
	P.w_u8(flags);
	fVector3				p = Position( );
	P.w_vec3(p);//Position());

	P.w_float /*w_angle8*/(angle_normalize(r_model_yaw)); //Device.vCameraDirection.getH());//
	P.w_float /*w_angle8*/(angle_normalize(unaffected_r_torso.yaw));//(r_torso.yaw);
	P.w_float /*w_angle8*/(angle_normalize(unaffected_r_torso.pitch));//(r_torso.pitch);
	P.w_float /*w_angle8*/(angle_normalize(unaffected_r_torso.roll));//(r_torso.roll);
	P.w_u8(u8(g_Team( )));
	P.w_u8(u8(g_Squad( )));
	P.w_u8(u8(g_Group( )));

	//CSE_ALifeCreatureTrader
//	P.w_float			(inventory().TotalWeight());
//	P.w_u32				(m_dwMoney);

	//CSE_ALifeCreatureActor

	u16 ms = (u16)(mstate_real & 0x0000ffff);
	P.w_u16(u16(ms));
	P.w_sdir(NET_SavedAccel);
	fVector3				v = character_physics_support( )->movement( )->GetVelocity( );
	P.w_sdir(v);//m_PhysicMovementControl.GetVelocity());
	P.w_float(g_Radiation( ));

	P.w_u8(u8(inventory( ).GetActiveSlot( )));
	/////////////////////////////////////////////////
	u16 NumItems = 0;

	P.w_u16(NumItems);
	if (!NumItems)
	{
		return;
	}

	if (g_Alive( ))
	{
		SPHNetState	State;

		CPHSynchronize* pSyncObj = NULL;
		pSyncObj = PHGetSyncItem(0);
		pSyncObj->get_State(State);

		P.w_u8(State.enabled);

		P.w_vec3(State.angular_vel);
		P.w_vec3(State.linear_vel);

		P.w_vec3(State.force);
		P.w_vec3(State.torque);

		P.w_vec3(State.position);

		P.w_float(State.quaternion.x);
		P.w_float(State.quaternion.y);
		P.w_float(State.quaternion.z);
		P.w_float(State.quaternion.w);
	}
	else
	{
		net_ExportDeadBody(P);
	}
}

static void w_vec_q8(CNetPacket& P, const fVector3& vec, const fVector3& min, const fVector3& max)
{
	P.w_float_q8(vec.x, min.x, max.x);
	P.w_float_q8(vec.y, min.y, max.y);
	P.w_float_q8(vec.z, min.z, max.z);
}
static void r_vec_q8(CNetPacket& P, fVector3& vec, const fVector3& min, const fVector3& max)
{
	P.r_float_q8(vec.x, min.x, max.x);
	P.r_float_q8(vec.y, min.y, max.y);
	P.r_float_q8(vec.z, min.z, max.z);

	clamp(vec.x, min.x, max.x);
	clamp(vec.y, min.y, max.y);
	clamp(vec.z, min.z, max.z);
}
static void w_qt_q8(CNetPacket& P, const fQuaternion& q)
{
	//fVector3 Q;
	//Q.set(q.x,q.y,q.z);
	//if(q.w<0.f)	Q.invert();
	//P.w_float_q8(Q.x,-1.f,1.f);
	//P.w_float_q8(Q.y,-1.f,1.f);
	//P.w_float_q8(Q.z,-1.f,1.f);
	///////////////////////////////////////////////////
	P.w_float_q8(q.x, -1.f, 1.f);
	P.w_float_q8(q.y, -1.f, 1.f);
	P.w_float_q8(q.z, -1.f, 1.f);
	P.w_float_q8(q.w, -1.f, 1.f);

	///////////////////////////////////////////


	//P.w_float_q8(q.x,-1.f,1.f);
	//P.w_float_q8(q.y,-1.f,1.f);
	//P.w_float_q8(q.z,-1.f,1.f);
	//P.w(sign())
}
static void r_qt_q8(CNetPacket& P, fQuaternion& q)
{
	//// x^2 + y^2 + z^2 + w^2 = 1
	//P.r_float_q8(q.x,-1.f,1.f);
	//P.r_float_q8(q.y,-1.f,1.f);
	//P.r_float_q8(q.z,-1.f,1.f);
	//f32 w2=1.f-q.x*q.x-q.y*q.y-q.z*q.z;
	//w2=w2<0.f ? 0.f : w2;
	//q.w=_sqrt(w2);
	/////////////////////////////////////////////////////
	///////////////////////////////////////////////////
	P.r_float_q8(q.x, -1.f, 1.f);
	P.r_float_q8(q.y, -1.f, 1.f);
	P.r_float_q8(q.z, -1.f, 1.f);
	P.r_float_q8(q.w, -1.f, 1.f);

	clamp(q.x, -1.f, 1.f);
	clamp(q.y, -1.f, 1.f);
	clamp(q.z, -1.f, 1.f);
	clamp(q.w, -1.f, 1.f);
}

#define F_MAX         3.402823466e+38F

static void	UpdateLimits(fVector3& p, fVector3& min, fVector3& max)
{
	if (p.x < min.x)
	{
		min.x = p.x;
	}

	if (p.y < min.y)
	{
		min.y = p.y;
	}

	if (p.z < min.z)
	{
		min.z = p.z;
	}

	if (p.x > max.x)
	{
		max.x = p.x;
	}

	if (p.y > max.y)
	{
		max.y = p.y;
	}

	if (p.z > max.z)
	{
		max.z = p.z;
	}

	for (s32 k = 0; k < 3; k++)
	{
		if (p[k]<min[k] || p[k]>max[k])
		{
			R_ASSERT2(0, "Fuck");
			UpdateLimits(p, min, max);
		}
	}
}

void CActor::net_ExportDeadBody(CNetPacket& P)
{
	/////////////////////////////
	fVector3 min;
	fVector3 max;

	min.set(F_MAX, F_MAX, F_MAX);
	max.set(-F_MAX, -F_MAX, -F_MAX);
	/////////////////////////////////////
	u16 bones_number = PHGetSyncItemsNumber( );
	for (u16 i = 0; i < bones_number; i++)
	{
		SPHNetState state;
		PHGetSyncItem(i)->get_State(state);

		fVector3& p = state.position;
		UpdateLimits(p, min, max);

		fVector3 px = state.linear_vel;
		px.div(10.0f);
		px.add(state.position);
		UpdateLimits(px, min, max);
	}

	P.w_u8(10);
	P.w_vec3(min);
	P.w_vec3(max);

	for (u16 i = 0; i < bones_number; i++)
	{
		SPHNetState state;
		PHGetSyncItem(i)->get_State(state);
		//		state.net_Save(P,min,max);
		w_vec_q8(P, state.position, min, max);
		w_qt_q8(P, state.quaternion);

		//---------------------------------
		fVector3 px = state.linear_vel;
		px.div(10.0f);
		px.add(state.position);
		w_vec_q8(P, px, min, max);
	}
}

void CActor::net_Import(CNetPacket& P)					// import from server
{
	//-----------------------------------------------
	net_Import_Base(P);
	//-----------------------------------------------

	m_u16NumBones = P.r_u16( );
	if (m_u16NumBones == 0)
	{
		return;
	}
	//-----------------------------------------------
	net_Import_Physic(P);
	//-----------------------------------------------
}

void CActor::net_Import_Base(CNetPacket& P)
{
	net_update			N;

	u8					flags;
	u16					tmp;

	//CSE_ALifeCreatureAbstract
	f32 health;
	P.r_float(health);
	//----------- for E3 -----------------------------
	if (OnClient( ))
	{
		SetfHealth(health);
	}
	//------------------------------------------------
	P.r_u32(N.dwTimeStamp);
	//---------------------------------------------

	//---------------------------------------------

	P.r_u8(flags);
	P.r_vec3(N.p_pos);
	P.r_float /*r_angle8*/(N.o_model);
	P.r_float /*r_angle8*/(N.o_torso.yaw);
	P.r_float /*r_angle8*/(N.o_torso.pitch);
	P.r_float /*r_angle8*/(N.o_torso.roll);
	if (N.o_torso.roll > PI)
	{
		N.o_torso.roll -= PI_MUL_2;
	}

	id_Team = P.r_u8( );
	id_Squad = P.r_u8( );
	id_Group = P.r_u8( );


	//----------- for E3 -----------------------------
//	if (OnClient())
	//------------------------------------------------
	{
		//		if (OnServer() || Remote())
		if (Level( ).IsDemoPlay( ))
		{
			unaffected_r_torso.yaw = N.o_torso.yaw;
			unaffected_r_torso.pitch = N.o_torso.pitch;
			unaffected_r_torso.roll = N.o_torso.roll;

			cam_Active( )->yaw = -N.o_torso.yaw;
			cam_Active( )->pitch = N.o_torso.pitch;
		}
	}

	//CSE_ALifeCreatureTrader
//	P.r_float			(fDummy);
//	m_dwMoney =			P.r_u32();

	//CSE_ALifeCreatureActor
	P.r_u16(tmp);
	N.mstate = u32(tmp);
	P.r_sdir(N.p_accel);
	P.r_sdir(N.p_velocity);
	f32				fRRadiation;
	P.r_float(fRRadiation);
	//----------- for E3 -----------------------------
	if (OnClient( ))
	{
		SetfRadiation(fRRadiation);
	}
	//------------------------------------------------

	u8					ActiveSlot;
	P.r_u8(ActiveSlot);

	//----------- for E3 -----------------------------
	if (OnClient( ))
		//------------------------------------------------
	{
		if (ActiveSlot == 0xff)
		{
			inventory( ).SetActiveSlot(NO_ACTIVE_SLOT);
		}
		else
		{
			if (inventory( ).GetActiveSlot( ) != u32(ActiveSlot))
			{
				inventory( ).Activate(u32(ActiveSlot));
			}
		}
	}

	//----------- for E3 -----------------------------
	if (Local( ) && OnClient( ))
	{
		return;
	}
	//-------------------------------------------------
	if (!NET.empty( ) && N.dwTimeStamp < NET.back( ).dwTimeStamp)
	{
		return;
	}

	if (!NET.empty( ) && N.dwTimeStamp == NET.back( ).dwTimeStamp)
	{
		NET.back( ) = N;
	}
	else
	{
		NET.push_back(N);
		if (NET.size( ) > 5)
		{
			NET.pop_front( );
		}
	}
	//-----------------------------------------------
	net_Import_Base_proceed( );
	//-----------------------------------------------
}

void CActor::net_Import_Base_proceed( )
{
	if (g_Alive( ))
	{
		setVisible((BOOL)!HUDview( ));
		setEnabled(TRUE);
	}
	//---------------------------------------------

	if (Remote( ))
	{
		return;
	}

	net_update N = NET.back( );
}

void CActor::net_Import_Physic(CNetPacket& P)
{
	m_States.clear( );
	if (m_u16NumBones != 1)
	{
		fVector3 min;
		fVector3 max;

		P.r_u8( );
		P.r_vec3(min);
		P.r_vec3(max);

		for (u16 i = 0; i < m_u16NumBones; i++)
		{
			SPHNetState state, stateL;
			PHGetSyncItem(i)->get_State(state);
			//			stateL.net_Load(P, min, max);
			r_vec_q8(P, stateL.position, min, max);
			r_qt_q8(P, stateL.quaternion);
			//---------------------------------------
			r_vec_q8(P, stateL.linear_vel, min, max);
			stateL.linear_vel.sub(stateL.position);
			stateL.linear_vel.mul(10.0f);
			//---------------------------------------
			state.position = stateL.position;
			state.previous_position = stateL.position;
			state.quaternion = stateL.quaternion;
			state.previous_quaternion = stateL.quaternion;
			state.linear_vel = stateL.linear_vel;
			//---------------------------------------
			m_States.push_back(state);
		}
	}
	else
	{
		net_update_A			N_A;

		P.r_u8(*((u8*)&(N_A.State.enabled)));

		P.r_vec3(N_A.State.angular_vel);
		P.r_vec3(N_A.State.linear_vel);

		P.r_vec3(N_A.State.force);
		P.r_vec3(N_A.State.torque);

		P.r_vec3(N_A.State.position);

		P.r_float(N_A.State.quaternion.x);
		P.r_float(N_A.State.quaternion.y);
		P.r_float(N_A.State.quaternion.z);
		P.r_float(N_A.State.quaternion.w);

		if (!NET.empty( ))
		{
			N_A.dwTimeStamp = NET.back( ).dwTimeStamp;
		}
		else
		{
			N_A.dwTimeStamp = Level( ).timeServer( );
		}

		N_A.State.previous_position = N_A.State.position;
		N_A.State.previous_quaternion = N_A.State.quaternion;
		//----------- for E3 -----------------------------
		if (Local( ) && OnClient( ) || !g_Alive( ))
		{
			return;
		}

		//		if (g_Alive() && (Remote() || OnServer()))
		{
			//-----------------------------------------------
			if (!NET_A.empty( ) && N_A.dwTimeStamp < NET_A.back( ).dwTimeStamp)
			{
				return;
			}

			if (!NET_A.empty( ) && N_A.dwTimeStamp == NET_A.back( ).dwTimeStamp)
			{
				NET_A.back( ) = N_A;
			}
			else
			{
				NET_A.push_back(N_A);
				if (NET_A.size( ) > 5) NET_A.pop_front( );
			}

			if (!NET_A.empty( ))
			{
				m_bInterpolate = true;
			}
		}
	}
	//-----------------------------------------------
	net_Import_Physic_proceed( );
	//-----------------------------------------------
}

void CActor::net_Import_Physic_proceed( )
{
	Level( ).AddObject_To_Objects4CrPr(this);
	CrPr_SetActivated(false);
	CrPr_SetActivationStep(0);
}

BOOL CActor::net_Spawn(CSE_Abstract* DC)
{
	m_holder_id = ALife::_OBJECT_ID(-1);
	m_feel_touch_characters = 0;
	m_snd_noise = 0.0f;
	m_sndShockEffector = NULL;
	if (m_pPhysicsShell)
	{
		m_pPhysicsShell->Deactivate( );
		xr_delete(m_pPhysicsShell);
	}
	//force actor to be local on server client
	CSE_Abstract* e = (CSE_Abstract*)(DC);
	CSE_ALifeCreatureActor* E = smart_cast<CSE_ALifeCreatureActor*>(e);
	if (OnServer( ))
	{
		E->s_flags.set(M_SPAWN_OBJECT_LOCAL, TRUE);
	}

	if (TRUE == E->s_flags.test(M_SPAWN_OBJECT_LOCAL) && TRUE == E->s_flags.is(M_SPAWN_OBJECT_ASPLAYER))
	{
		g_actor = this;
	}

	VERIFY(m_pActorEffector == NULL);
	m_pActorEffector = xr_new<CCameraManager>(false);

	// motions
	m_bAnimTorsoPlayed = false;
	m_current_legs_blend = 0;
	m_current_jump_blend = 0;
	m_current_legs.invalidate( );
	m_current_torso.invalidate( );
	m_current_head.invalidate( );
	//-------------------------------------
	// ������������� ��������, ������������ �������
	encyclopedia_registry->registry( ).init(ID( ));
	game_news_registry->registry( ).init(ID( ));

	if (!CInventoryOwner::net_Spawn(DC))
	{
		return FALSE;
	}

	if (!inherited::net_Spawn(DC))
	{
		return FALSE;
	}

	CSE_ALifeTraderAbstract* pTA = smart_cast<CSE_ALifeTraderAbstract*>(e);
	set_money(pTA->m_dwMoney, false);

	//������ ��� ��������� � �����
	m_ArtefactsOnBelt.clear( );
	//.	if(	TRUE == E->s_flags.test(M_SPAWN_OBJECT_LOCAL) && TRUE == E->s_flags.is(M_SPAWN_OBJECT_ASPLAYER))
	//.		HUD().GetUI()->UIMainIngameWnd->m_artefactPanel->InitIcons(m_ArtefactsOnBelt);

	ROS( )->force_mode(IRender_ObjectSpecific::TRACE_ALL);

	m_pPhysics_support->in_NetSpawn(e);
	character_physics_support( )->movement( )->ActivateBox(0);
	if (E->m_holderID != u16(-1))
	{
		character_physics_support( )->movement( )->DestroyCharacter( );
	}

	if (m_bOutBorder)
	{
		character_physics_support( )->movement( )->setOutBorder( );
	}

	r_torso_tgt_roll = 0.0f;

	r_model_yaw = E->o_torso.yaw;
	r_torso.yaw = E->o_torso.yaw;
	r_torso.pitch = E->o_torso.pitch;
	r_torso.roll = 0.0f;//E->o_Angle.z;

	unaffected_r_torso.yaw = r_torso.yaw;
	unaffected_r_torso.pitch = r_torso.pitch;
	unaffected_r_torso.roll = r_torso.roll;

	if (psActorFlags.test(AF_PSP))
	{
		cam_Set(eacLookAt);
	}
	else
	{
		cam_Set(eacFirstEye);
	}

	cam_Active( )->Set(-E->o_torso.yaw, E->o_torso.pitch, 0);//E->o_Angle.z);

	// *** movement state - respawn
	mstate_wishful = 0;
	mstate_real = 0;
	mstate_old = 0;
	m_bJumpKeyPressed = FALSE;

	NET_SavedAccel.set(0.0f, 0.0f, 0.0f);
	NET_WasInterpolating = TRUE;

	setEnabled(E->s_flags.is(M_SPAWN_OBJECT_LOCAL));

	Engine.Sheduler.Register(this, TRUE);

	hit_slowmo = 0.0f;

	OnChangeVisual( );
	//----------------------------------
	m_bAllowDeathRemove = false;

	m_bInInterpolation = false;
	m_bInterpolate = false;

	processing_activate( );

#ifdef DEBUG
	LastPosS.clear( );
	LastPosH.clear( );
	LastPosL.clear( );
#endif

	//*

	//	if (OnServer())// && E->s_flags.is(M_SPAWN_OBJECT_LOCAL))
	/*
		if (OnClient())
		{
			if (!pStatGraph)
			{
				static g_Y = 0;
				pStatGraph = xr_new<CStatGraph>();
				pStatGraph->SetRect(0, g_Y, Device.dwWidth, 100, 0xff000000, 0xff000000);
				g_Y += 110;
				if (g_Y > 700) g_Y = 100;
				pStatGraph->SetGrid(0, 0.0f, 10, 1.0f, 0xff808080, 0xffffffff);
				pStatGraph->SetMinMax(0, 10, 300);
				pStatGraph->SetStyle(CStatGraph::stBar);
				pStatGraph->AppendSubGraph(CStatGraph::stCurve);
				pStatGraph->AppendSubGraph(CStatGraph::stCurve);
			}
		}
	*/
	SetDefaultVisualOutfit(cNameVisual( ));

	smart_cast<CKinematics*>(Visual( ))->CalculateBones( );

	//--------------------------------------------------------------
	inventory( ).SetPrevActiveSlot(NO_ACTIVE_SLOT);

	//-------------------------------------
	m_States.empty( );
	//-------------------------------------
	if (!g_Alive( ))
	{
		mstate_wishful &= ~mcAnyMove;
		mstate_real &= ~mcAnyMove;
		CKinematicsAnimated* K = smart_cast<CKinematicsAnimated*>(Visual( ));
		K->PlayCycle("death_init");

		//���������� ���� �������� �������
		m_HeavyBreathSnd.stop( );
	}

	typedef CClientSpawnManager::CALLBACK_TYPE	CALLBACK_TYPE;
	CALLBACK_TYPE	callback;
	callback.bind(this, &CActor::on_requested_spawn);
	m_holder_id = E->m_holderID;
	if (E->m_holderID != ALife::_OBJECT_ID(-1))
	{
		Level( ).client_spawn_manager( ).add(E->m_holderID, ID( ), callback);
	}
	//F
	//-------------------------------------------------------------
	m_iLastHitterID = u16(-1);
	m_iLastHittingWeaponID = u16(-1);
	m_s16LastHittedElement = -1;
	m_bWasHitted = false;
	m_dwILastUpdateTime = 0;

	Level( ).MapManager( ).AddMapLocation("actor_location", ID( ));
	Level( ).MapManager( ).AddMapLocation("actor_location_p", ID( ));

	m_game_task_manager = xr_new<CGameTaskManager>( );
	GameTaskManager( ).initialize(ID( ));

	m_statistic_manager = xr_new<CActorStatisticManager>( );

	spatial.type |= STYPE_REACTTOSOUND;
	psHUD_Flags.set(HUD_WEAPON_RT, TRUE);

	if (Level( ).IsDemoPlay( ) && OnClient( ))
	{
		setLocal(FALSE);
	}

	return					TRUE;
}

void CActor::net_Destroy( )
{
	inherited::net_Destroy( );

	if (m_holder_id != ALife::_OBJECT_ID(-1))
	{
		Level( ).client_spawn_manager( ).remove(m_holder_id, ID( ));
	}

	delete_data(m_game_task_manager);
	delete_data(m_statistic_manager);

	Level( ).MapManager( ).RemoveMapLocationByObjectID(ID( ));

#pragma todo("Dima to MadMax : do not comment inventory owner net_Destroy!!!")
	CInventoryOwner::net_Destroy( );
	cam_UnsetLadder( );
	character_physics_support( )->movement( )->DestroyCharacter( );
	if (m_pPhysicsShell)
	{
		m_pPhysicsShell->Deactivate( );
		xr_delete<CPhysicsShell>(m_pPhysicsShell);
	}

	m_pPhysics_support->in_NetDestroy( );

	xr_delete(m_sndShockEffector);
	xr_delete(pStatGraph);
	xr_delete(m_pActorEffector);
	pCamBobbing = NULL;

#ifdef DEBUG	
	LastPosS.clear( );
	LastPosH.clear( );
	LastPosL.clear( );
#endif

	processing_deactivate( );
	m_holder = NULL;
	m_holderID = u16(-1);

	m_ArtefactsOnBelt.clear( );
	if (Level( ).CurrentViewEntity( ) == this)
	{
		HUD( ).GetUI( )->UIMainIngameWnd->m_artefactPanel->InitIcons(m_ArtefactsOnBelt);
	}

	SetDefaultVisualOutfit(NULL);

	if (g_actor == this)
	{
		g_actor = NULL;
	}

	Engine.Sheduler.Unregister(this);
}

void CActor::net_Relcase(CObject* O)
{

	VERIFY(O);
	CGameObject* GO = smart_cast<CGameObject*>(O);
	if (GO && m_pObjectWeLookingAt == GO)
	{
		m_pObjectWeLookingAt = NULL;
	}

	CHolderCustom* HC = smart_cast<CHolderCustom*>(GO);
	if (HC && HC == m_pVehicleWeLookingAt)
	{
		m_pVehicleWeLookingAt = NULL;
	}

	if (HC && HC == m_holder)
	{
		m_holder->detach_Actor( );
		m_holder = NULL;
	}

	inherited::net_Relcase(O);

	memory( ).remove_links(O);
	m_pPhysics_support->in_NetRelcase(O);
}

BOOL CActor::net_Relevant( )				// relevant for export to server
{
	if (OnServer( ))
	{
		return getSVU( ) | getLocal( );
	}
	else
	{
		return Local( ) & g_Alive( );
	}
}

void CActor::SetCallbacks( )
{
	CKinematics* V = smart_cast<CKinematics*>(Visual( ));
	VERIFY(V);
	u16 spine0_bone = V->LL_BoneID("bip01_spine");
	u16 spine1_bone = V->LL_BoneID("bip01_spine1");
	u16 shoulder_bone = V->LL_BoneID("bip01_spine2");
	u16 head_bone = V->LL_BoneID("bip01_head");
	V->LL_GetBoneInstance(u16(spine0_bone)).set_callback(bctCustom, Spin0Callback, this);
	V->LL_GetBoneInstance(u16(spine1_bone)).set_callback(bctCustom, Spin1Callback, this);
	V->LL_GetBoneInstance(u16(shoulder_bone)).set_callback(bctCustom, ShoulderCallback, this);
	V->LL_GetBoneInstance(u16(head_bone)).set_callback(bctCustom, HeadCallback, this);
}

void CActor::ResetCallbacks( )
{
	CKinematics* V = smart_cast<CKinematics*>(Visual( ));
	VERIFY(V);
	u16 spine0_bone = V->LL_BoneID("bip01_spine");
	u16 spine1_bone = V->LL_BoneID("bip01_spine1");
	u16 shoulder_bone = V->LL_BoneID("bip01_spine2");
	u16 head_bone = V->LL_BoneID("bip01_head");
	V->LL_GetBoneInstance(u16(spine0_bone)).reset_callback( );
	V->LL_GetBoneInstance(u16(spine1_bone)).reset_callback( );
	V->LL_GetBoneInstance(u16(shoulder_bone)).reset_callback( );
	V->LL_GetBoneInstance(u16(head_bone)).reset_callback( );
}

void CActor::OnChangeVisual( )
{
	CPhysicsShell* tmp_shell = PPhysicsShell( );
	PPhysicsShell( ) = NULL;
	inherited::OnChangeVisual( );
	PPhysicsShell( ) = tmp_shell;
	tmp_shell = NULL;

	CKinematicsAnimated* V = smart_cast<CKinematicsAnimated*>(Visual( ));
	if (V)
	{
		SetCallbacks( );
		m_anims->Create(V);
		m_vehicle_anims->Create(V);
		CDamageManager::reload(*cNameSect( ), "damage", pSettings);
		//-------------------------------------------------------------------------------
		m_head = smart_cast<CKinematics*>(Visual( ))->LL_BoneID("bip01_head");
		m_r_hand = smart_cast<CKinematics*>(Visual( ))->LL_BoneID(pSettings->r_string(*cNameSect( ), "weapon_bone0"));
		m_l_finger1 = smart_cast<CKinematics*>(Visual( ))->LL_BoneID(pSettings->r_string(*cNameSect( ), "weapon_bone1"));
		m_r_finger2 = smart_cast<CKinematics*>(Visual( ))->LL_BoneID(pSettings->r_string(*cNameSect( ), "weapon_bone2"));
		//-------------------------------------------------------------------------------
		m_neck = smart_cast<CKinematics*>(Visual( ))->LL_BoneID("bip01_neck");
		m_l_clavicle = smart_cast<CKinematics*>(Visual( ))->LL_BoneID("bip01_l_clavicle");
		m_r_clavicle = smart_cast<CKinematics*>(Visual( ))->LL_BoneID("bip01_r_clavicle");
		m_spine2 = smart_cast<CKinematics*>(Visual( ))->LL_BoneID("bip01_spine2");
		m_spine1 = smart_cast<CKinematics*>(Visual( ))->LL_BoneID("bip01_spine1");
		m_spine = smart_cast<CKinematics*>(Visual( ))->LL_BoneID("bip01_spine");
		//-------------------------------------------------------------------------------
		reattach_items( );
		//-------------------------------------------------------------------------------
		m_pPhysics_support->in_ChangeVisual( );
		//-------------------------------------------------------------------------------
		SetCallbacks( );
		//-------------------------------------------------------------------------------
		m_current_head.invalidate( );
		m_current_legs.invalidate( );
		m_current_torso.invalidate( );
		m_current_legs_blend = NULL;
		m_current_torso_blend = NULL;
		m_current_jump_blend = NULL;
	}
}

void CActor::ChangeVisual(shared_str NewVisual)
{
	if (!NewVisual.size( ))
	{
		return;
	}

	if (cNameVisual( ).size( ))
	{
		if (cNameVisual( ) == NewVisual)
		{
			return;
		}
	}

	cNameVisual_set(NewVisual);

	g_SetAnimation(mstate_real);
	Visual( )->dcast_PKinematics( )->CalculateBones_Invalidate( );
	Visual( )->dcast_PKinematics( )->CalculateBones( );
}

InterpData				IStartT;
InterpData				IRecT;
InterpData				IEndT;

void CActor::PH_B_CrPr( )	// actions & operations before physic correction-prediction steps
{
	//just set last update data for now
	if (CrPr_IsActivated( ))
	{
		return;
	}

	if (CrPr_GetActivationStep( ) > ph_world->m_steps_num)
	{
		return;
	}

	if (g_Alive( ))
	{
		CrPr_SetActivated(true);
		{
			///////////////////////////////////////////////
			InterpData* pIStart = &IStart;
			pIStart->Pos = Position( );
			pIStart->Vel = character_physics_support( )->movement( )->GetVelocity( );
			pIStart->o_model = angle_normalize(r_model_yaw);
			pIStart->o_torso.yaw = angle_normalize(unaffected_r_torso.yaw);
			pIStart->o_torso.pitch = angle_normalize(unaffected_r_torso.pitch);
			pIStart->o_torso.roll = angle_normalize(unaffected_r_torso.roll);
			if (pIStart->o_torso.roll > PI)
			{
				pIStart->o_torso.roll -= PI_MUL_2;
			}
		}

		///////////////////////////////////////////////
		CPHSynchronize* pSyncObj = NULL;
		pSyncObj = PHGetSyncItem(0);
		if (!pSyncObj)
		{
			return;
		}

		pSyncObj->get_State(LastState);
		///////////////////////////////////////////////

		//----------- for E3 -----------------------------
		if (Local( ) && OnClient( ))
			//------------------------------------------------
		{
			PHUnFreeze( );

			pSyncObj->set_State(NET_A.back( ).State);
		}
		else
		{
			net_update_A N_A = NET_A.back( );
			net_update N = NET.back( );

			NET_Last = N;
			///////////////////////////////////////////////
			cam_Active( )->Set(-unaffected_r_torso.yaw, unaffected_r_torso.pitch, 0);//, unaffected_r_torso.roll);		// set's camera orientation
			if (!N_A.State.enabled)
			{
				pSyncObj->set_State(N_A.State);
			}
			else
			{
				PHUnFreeze( );

				pSyncObj->set_State(N_A.State);

				g_Physics(N.p_accel, 0.0f, 0.0f);
				Position( ).set(IStart.Pos);
			}
		}
	}
	else
	{
		if (PHGetSyncItemsNumber( ) != m_u16NumBones || m_States.empty( ))
		{
			return;
		}

		CrPr_SetActivated(true);

		PHUnFreeze( );

		for (u16 i = 0; i < m_u16NumBones; i++)
		{
			SPHNetState state, stateL;
			PHGetSyncItem(i)->get_State(state);
			stateL = m_States[i];
			//---------------------------------------
			state.position = stateL.position;
			state.previous_position = stateL.previous_position;
			state.quaternion = stateL.quaternion;
			state.previous_quaternion = stateL.previous_quaternion;
			state.linear_vel = stateL.linear_vel;
			state.enabled = true;
			//---------------------------------------
			PHGetSyncItem(i)->set_State(state);
		}
	}
}

void CActor::PH_I_CrPr( )		// actions & operations between two phisic prediction steps
{
	//store recalculated data, then we able to restore it after small future prediction
	if (!CrPr_IsActivated( ))
	{
		return;
	}

	if (g_Alive( ))
	{
		////////////////////////////////////
		CPHSynchronize* pSyncObj = NULL;
		pSyncObj = PHGetSyncItem(0);
		if (!pSyncObj)
		{
			return;
		}

		////////////////////////////////////
		pSyncObj->get_State(RecalculatedState);
		////////////////////////////////////
	}
}

void CActor::PH_A_CrPr( )
{
	//restore recalculated data and get data for interpolation	
	if (!CrPr_IsActivated( ))
	{
		return;
	}

	if (!g_Alive( ))
	{
		return;
	}
	////////////////////////////////////
	CPHSynchronize* pSyncObj = NULL;
	pSyncObj = PHGetSyncItem(0);
	if (!pSyncObj)
	{
		return;
	}
	////////////////////////////////////
	pSyncObj->get_State(PredictedState);
	////////////////////////////////////
	pSyncObj->set_State(RecalculatedState);
	////////////////////////////////////
	if (!m_bInterpolate)
	{
		return;
	}
	////////////////////////////////////
	mstate_wishful = mstate_real = NET_Last.mstate;
	CalculateInterpolationParams( );
}
extern f32 g_cl_lvInterp;

void CActor::CalculateInterpolationParams( )
{
	//	fMatrix4x4 xformX0, xformX1;
	CPHSynchronize* pSyncObj = NULL;
	pSyncObj = PHGetSyncItem(0);
	///////////////////////////////////////////////
	InterpData* pIStart = &IStart;
	InterpData* pIRec = &IRec;
	InterpData* pIEnd = &IEnd;

	///////////////////////////////////////////////
	/*
	pIStart->Pos				= Position();
	pIStart->Vel				= m_PhysicMovementControl->GetVelocity();
	pIStart->o_model			= r_model_yaw;
	pIStart->o_torso.yaw		= unaffected_r_torso.yaw;
	pIStart->o_torso.pitch		= unaffected_r_torso.pitch;
	pIStart->o_torso.roll		= unaffected_r_torso.roll;
	*/
	/////////////////////////////////////////////////////////////////////
	pIRec->Pos = RecalculatedState.position;
	pIRec->Vel = RecalculatedState.linear_vel;
	pIRec->o_model = NET_Last.o_model;
	pIRec->o_torso = NET_Last.o_torso;
	/////////////////////////////////////////////////////////////////////
	pIEnd->Pos = PredictedState.position;
	pIEnd->Vel = PredictedState.linear_vel;
	pIEnd->o_model = pIRec->o_model;
	pIEnd->o_torso.yaw = pIRec->o_torso.yaw;
	pIEnd->o_torso.pitch = pIRec->o_torso.pitch;
	pIEnd->o_torso.roll = pIRec->o_torso.roll;
	/////////////////////////////////////////////////////////////////////
//	Msg("from %f, to %f", IStart.o_torso.yaw/PI*180.0f, IEnd.o_torso.yaw/PI*180.0f);
	/////////////////////////////////////////////////////////////////////
	fVector3 SP0;
	fVector3 SP1;
	fVector3 SP2;
	fVector3 SP3;
	fVector3 HP0;
	fVector3 HP1;
	fVector3 HP2;
	fVector3 HP3;

	SP0 = pIStart->Pos;
	HP0 = pIStart->Pos;

	if (m_bInInterpolation)
	{
		u32 CurTime = Level( ).timeServer( );
		f32 factor = f32(CurTime - m_dwIStartTime) / (m_dwIEndTime - m_dwIStartTime);
		if (factor > 1.0f)
		{
			factor = 1.0f;
		}

		f32 c = factor;
		for (u32 k = 0; k < 3; k++)
		{
			SP0[k] = c * (c * (c * SCoeff[k][0] + SCoeff[k][1]) + SCoeff[k][2]) + SCoeff[k][3];
			SP1[k] = (c * c * SCoeff[k][0] * 3 + c * SCoeff[k][1] * 2 + SCoeff[k][2]) / 3; // �������� �� ������� � 3 ���� ��������� �������� ��� ������� ������������� !!!!

			HP0[k] = c * (c * (c * HCoeff[k][0] + HCoeff[k][1]) + HCoeff[k][2]) + HCoeff[k][3];
			HP1[k] = (c * c * HCoeff[k][0] * 3 + c * HCoeff[k][1] * 2 + HCoeff[k][2]); // �������� �� ������� � 3 ���� ��������� �������� ��� ������� ������������� !!!!
		}

		SP1.add(SP0);
	}
	else
	{
		if (LastState.linear_vel.x == 0 && LastState.linear_vel.y == 0 && LastState.linear_vel.z == 0)
		{
			HP1.sub(RecalculatedState.position, RecalculatedState.previous_position);
		}
		else
		{
			HP1.sub(LastState.position, LastState.previous_position);
		}

		HP1.mul(1.0f / fixed_step);
		SP1.add(HP1, SP0);
	}

	HP2.sub(PredictedState.position, PredictedState.previous_position);
	HP2.mul(1.0f / fixed_step);
	SP2.sub(PredictedState.position, HP2);

	SP3.set(PredictedState.position);
	HP3.set(PredictedState.position);
	/*
	{
	fVector3 d0, d1;
	d0.sub(SP1, SP0);
	d1.sub(SP3, SP0);
	f32 res = d0.dotproduct(d1);
	if (res < 0)
	{
	Msg ("! %f", res);
	}
	else
	Msg ("%f", res);
	}
	*/
	/////////////////////////////////////////////////////////////////////////////
	fVector3 TotalPath;
	TotalPath.sub(SP3, SP0);
	f32 TotalLen = TotalPath.magnitude( );

	SPHNetState	State0 = (NET_A.back( )).State;
	SPHNetState	State1 = PredictedState;

	f32 lV0 = State0.linear_vel.magnitude( );
	f32 lV1 = State1.linear_vel.magnitude( );

	u32		ConstTime = u32((fixed_step - ph_world->m_frame_time) * 1000) + Level( ).GetInterpolationSteps( ) * u32(fixed_step * 1000);

	m_dwIStartTime = m_dwILastUpdateTime;

	//	if (( lV0 + lV1) > 0.000001 && g_cl_lvInterp == 0)
	{
		//		u32		CulcTime = iCeil(TotalLen*2000/( lV0 + lV1));
		//		m_dwIEndTime = m_dwIStartTime + min(CulcTime, ConstTime);
	}
	//	else
	m_dwIEndTime = m_dwIStartTime + ConstTime;
	/////////////////////////////////////////////////////////////////////////////
	fVector3 V0;
	fVector3 V1;
	//	V0.sub(SP1, SP0);
	//	V1.sub(SP3, SP2);
	V0.set(HP1);
	V1.set(HP2);
	lV0 = V0.magnitude( );
	lV1 = V1.magnitude( );

	if (TotalLen != 0)
	{
		if (V0.x != 0 || V0.y != 0 || V0.z != 0)
		{
			if (lV0 > TotalLen / 3)
			{
				HP1.normalize( );
				//				V0.normalize();
				//				V0.mul(TotalLen/3);
				HP1.normalize( );
				HP1.mul(TotalLen / 3);
				SP1.add(HP1, SP0);
			}
		}

		if (V1.x != 0 || V1.y != 0 || V1.z != 0)
		{
			if (lV1 > TotalLen / 3)
			{
				//				V1.normalize();
				//				V1.mul(TotalLen/3);
				HP2.normalize( );
				HP2.mul(TotalLen / 3);
				SP2.sub(SP3, HP2);
			}
		}
	}
	/////////////////////////////////////////////////////////////////////////////
	for (u32 i = 0; i < 3; i++)
	{
		SCoeff[i][0] = SP3[i] - 3 * SP2[i] + 3 * SP1[i] - SP0[i];
		SCoeff[i][1] = 3 * SP2[i] - 6 * SP1[i] + 3 * SP0[i];
		SCoeff[i][2] = 3 * SP1[i] - 3 * SP0[i];
		SCoeff[i][3] = SP0[i];

		HCoeff[i][0] = 2 * HP0[i] - 2 * HP3[i] + HP1[i] + HP2[i];
		HCoeff[i][1] = -3 * HP0[i] + 3 * HP3[i] - 2 * HP1[i] - HP2[i];
		HCoeff[i][2] = HP1[i];
		HCoeff[i][3] = HP0[i];
	}
	/////////////////////////////////////////////////////////////////////////////
	m_bInInterpolation = true;

	if (m_pPhysicsShell)
	{
		m_pPhysicsShell->NetInterpolationModeON( );
	}
}

s32		actInterpType = 0;
void CActor::make_Interpolation( )
{
	m_dwILastUpdateTime = Level( ).timeServer( );

	if (g_Alive( ) && m_bInInterpolation)
	{
		u32 CurTime = m_dwILastUpdateTime;

		if (CurTime >= m_dwIEndTime)
		{
			m_bInInterpolation = false;
			mstate_real = mstate_wishful = NET_Last.mstate;
			NET_SavedAccel = NET_Last.p_accel;

			CPHSynchronize* pSyncObj = NULL;
			pSyncObj = PHGetSyncItem(0);
			if (!pSyncObj)
			{
				return;
			}

			pSyncObj->set_State(PredictedState);//, PredictedState.enabled);
			VERIFY2(_valid(renderable.xform), *cName( ));
		}
		else
		{
			f32 factor = 0.0f;

			if (m_dwIEndTime != m_dwIStartTime)
			{
				factor = f32(CurTime - m_dwIStartTime) / (m_dwIEndTime - m_dwIStartTime);
			}

			fVector3 NewPos;
			NewPos.lerp(IStart.Pos, IEnd.Pos, factor);

			VERIFY2(_valid(renderable.xform), *cName( ));

			//			r_model_yaw		= angle_lerp	(IStart.o_model,IEnd.o_model,		factor);			
			unaffected_r_torso.yaw = angle_lerp(IStart.o_torso.yaw, IEnd.o_torso.yaw, factor);
			unaffected_r_torso.pitch = angle_lerp(IStart.o_torso.pitch, IEnd.o_torso.pitch, factor);
			unaffected_r_torso.roll = angle_lerp(IStart.o_torso.roll, IEnd.o_torso.roll, factor);

			for (u32 k = 0; k < 3; k++)
			{
				IPosL[k] = NewPos[k];
				IPosS[k] = factor * (factor * (factor * SCoeff[k][0] + SCoeff[k][1]) + SCoeff[k][2]) + SCoeff[k][3];
				IPosH[k] = factor * (factor * (factor * HCoeff[k][0] + HCoeff[k][1]) + HCoeff[k][2]) + HCoeff[k][3];
			}

			fVector3 SpeedVector;
			fVector3 ResPosition;
			switch (g_cl_InterpolationType)
			{
				case 0:
				{
					ResPosition.set(IPosL);
					SpeedVector.sub(IEnd.Pos, IStart.Pos);
					SpeedVector.div(f32(m_dwIEndTime - m_dwIStartTime) / 1000.0f);
				}
				break;
				case 1:
				{
					for (s32 k = 0; k < 3; k++)
					{
						SpeedVector[k] = (factor * factor * SCoeff[k][0] * 3 + factor * SCoeff[k][1] * 2 + SCoeff[k][2]) / 3; // �������� �� ������� � 3 ���� ��������� �������� ��� ������� ������������� !!!!
					}

					ResPosition.set(IPosS);
				}
				break;
				case 2:
				{
					for (s32 k = 0; k < 3; k++)
					{
						SpeedVector[k] = (factor * factor * HCoeff[k][0] * 3 + factor * HCoeff[k][1] * 2 + HCoeff[k][2]);
					}

					ResPosition.set(IPosH);
				}
				break;
				default:
				{
					R_ASSERT2(0, "Unknown interpolation curve type!");
				}
			}

			character_physics_support( )->movement( )->SetPosition(ResPosition);
			character_physics_support( )->movement( )->SetVelocity(SpeedVector);
			cam_Active( )->Set(-unaffected_r_torso.yaw, unaffected_r_torso.pitch, 0);//, unaffected_r_torso.roll);
		}
	}
	else
	{
		m_bInInterpolation = false;
	}

#ifdef DEBUG
	if (getVisible( ) && g_Alive( ) && mstate_real)
	{
		LastPosS.push_back(IPosS);	while (LastPosS.size( ) > g_cl_InterpolationMaxPoints) LastPosS.pop_front( );
		LastPosH.push_back(IPosH);	while (LastPosH.size( ) > g_cl_InterpolationMaxPoints) LastPosH.pop_front( );
		LastPosL.push_back(IPosL);	while (LastPosL.size( ) > g_cl_InterpolationMaxPoints) LastPosL.pop_front( );
	}
#endif
}

void CActor::save(CNetPacket& output_packet)
{
	inherited::save(output_packet);
	CInventoryOwner::save(output_packet);
	output_packet.w_u8(u8(m_bOutBorder));
}

void CActor::load(IReader& input_packet)
{
	inherited::load(input_packet);
	CInventoryOwner::load(input_packet);
	m_bOutBorder = !!(input_packet.r_u8( ));
}

#ifdef DEBUG
extern	flags32	dbg_net_Draw_Flags;
void dbg_draw_piramid(fVector3 pos, fVector3 dir, f32 size, f32 xdir, u32 color)
{
	fVector3 p0;
	fVector3 p1;
	fVector3 p2;
	fVector3 p3;
	fVector3 p4;
	p0.set(size, size, 0.0f);
	p1.set(-size, size, 0.0f);
	p2.set(-size, -size, 0.0f);
	p3.set(size, -size, 0.0f);
	p4.set(0.0f, 0.0f, size * 4.0f);

	bool Double = false;
	fMatrix4x4 t;
	t.identity( );
	if (_valid(dir) && dir.square_magnitude( ) > 0.01f)
	{
		t.k.normalize(dir);
		fVector3::generate_orthonormal_basis(t.k, t.j, t.i);
	}
	else
	{
		t.rotateY(xdir);
		Double = true;
	}

	t.c.set(pos);

	if (!Double)
	{
		RCache.dbg_DrawTRI(t, p0, p1, p4, color);
		RCache.dbg_DrawTRI(t, p1, p2, p4, color);
		RCache.dbg_DrawTRI(t, p2, p3, p4, color);
		RCache.dbg_DrawTRI(t, p3, p0, p4, color);
	}
	else
	{
		Level( ).debug_renderer( ).draw_line(t, p0, p1, color);
		Level( ).debug_renderer( ).draw_line(t, p1, p2, color);
		Level( ).debug_renderer( ).draw_line(t, p2, p3, color);
		Level( ).debug_renderer( ).draw_line(t, p3, p0, color);

		Level( ).debug_renderer( ).draw_line(t, p0, p4, color);
		Level( ).debug_renderer( ).draw_line(t, p1, p4, color);
		Level( ).debug_renderer( ).draw_line(t, p2, p4, color);
		Level( ).debug_renderer( ).draw_line(t, p3, p4, color);
	}
}

void CActor::OnRender_Network( )
{
	RCache.OnFrameEnd( );

	//-----------------------------------------------------------------------------------------------------
	f32 size = 0.2f;

	//	dbg_draw_piramid(Position(), m_PhysicMovementControl->GetVelocity(), size/2, -r_model_yaw, color_rgba(255, 255, 255, 255));
		//-----------------------------------------------------------------------------------------------------
	if (g_Alive( ))
	{
		if (dbg_net_Draw_Flags.test(1 << 8))
		{
			fVector3 bc;
			bc.add(Position( ), m_AutoPickUp_AABB_Offset);
			fVector3 bd = m_AutoPickUp_AABB;

			Level( ).debug_renderer( ).draw_aabb(bc, bd.x, bd.y, bd.z, color_rgba(0, 255, 0, 255));
		}

		CKinematics* V = smart_cast<CKinematics*>(Visual( ));
		if (dbg_net_Draw_Flags.test(1 << 0) && V)
		{
			if (this != Level( ).CurrentViewEntity( ) || cam_active != eacFirstEye)
			{
				/*
				u16 BoneCount = V->LL_BoneCount();
				for (u16 i=0; i<BoneCount; i++)
				{
					fObb BoneOBB = V->LL_GetBox(i);
					fMatrix4x4 BoneMatrix; BoneOBB.xform_get(BoneMatrix);
					fMatrix4x4 BoneMatrixRes; BoneMatrixRes.mul(V->LL_GetTransform(i), BoneMatrix);
					BoneMatrix.mul(XFORM(), BoneMatrixRes);
					Level().debug_renderer().draw_obb(BoneMatrix, BoneOBB.m_halfsize, color_rgba(0, 255, 0, 255));
				};
				*/
				CCF_Skeleton* Skeleton = smart_cast<CCF_Skeleton*>(collidable.model);
				if (Skeleton)
				{
					Skeleton->_dbg_refresh( );

					const CCF_Skeleton::ElementVec& Elements = Skeleton->_GetElements( );
					for (CCF_Skeleton::ElementVec::const_iterator I = Elements.begin( ); I != Elements.end( ); I++)
					{
						if (!I->valid( ))
						{
							continue;
						}

						switch (I->type)
						{
							case SBoneShape::stBox:
							{
								fMatrix4x4 M;
								M.invert(I->b_IM);
								fVector3 h_size = I->b_hsize;
								Level( ).debug_renderer( ).draw_obb(M, h_size, color_rgba(0, 255, 0, 255));
							}
							break;
							case SBoneShape::stCylinder:
							{
								fMatrix4x4 M;
								M.c.set(I->c_cylinder.m_center);
								M.k.set(I->c_cylinder.m_direction);
								fVector3 h_size;
								h_size.set(I->c_cylinder.m_radius, I->c_cylinder.m_radius, I->c_cylinder.m_height * 0.5f);
								fVector3::generate_orthonormal_basis(M.k, M.j, M.i);
								Level( ).debug_renderer( ).draw_obb(M, h_size, color_rgba(0, 127, 255, 255));
							}
							break;
							case SBoneShape::stSphere:
							{
								fMatrix4x4 l_ball;
								l_ball.scale(I->s_sphere.R, I->s_sphere.R, I->s_sphere.R);
								l_ball.translate_add(I->s_sphere.P);
								Level( ).debug_renderer( ).draw_ellipse(l_ball, color_rgba(0, 255, 0, 255));
							}
							break;
						}
					}
				}
			}
		}

		if (!(dbg_net_Draw_Flags.is_any((1 << 1))))
		{
			return;
		}

		dbg_draw_piramid(Position( ), character_physics_support( )->movement( )->GetVelocity( ), size, -r_model_yaw, color_rgba(128, 255, 128, 255));
		dbg_draw_piramid(IStart.Pos, IStart.Vel, size, -IStart.o_model, color_rgba(255, 0, 0, 255));
		dbg_draw_piramid(IRec.Pos, IRec.Vel, size, -IRec.o_model, color_rgba(0, 0, 255, 255));
		dbg_draw_piramid(IEnd.Pos, IEnd.Vel, size, -IEnd.o_model, color_rgba(0, 255, 0, 255));
		dbg_draw_piramid(NET_Last.p_pos, NET_Last.p_velocity, size * 3 / 4, -NET_Last.o_model, color_rgba(255, 255, 255, 255));

		fMatrix4x4 MS;
		fMatrix4x4 MH;
		fMatrix4x4 ML;
		fMatrix4x4* pM = NULL;
		ML.translate(0.0f, 0.2f, 0.0f);
		MS.translate(0.0f, 0.2f, 0.0f);
		MH.translate(0.0f, 0.2f, 0.0f);

		fVector3 point0S;
		fVector3 point1S;
		fVector3 point0H;
		fVector3 point1H;
		fVector3 point0L;
		fVector3 point1L;
		fVector3* ppoint0 = NULL;
		fVector3* ppoint1 = NULL;
		fVector3 tS;
		fVector3 tH;
		u32	cColor = 0;
		u32 sColor = 0;
		VIS_POSITION* pLastPos = NULL;

		switch (g_cl_InterpolationType)
		{
			case 0:
			{
				ppoint0 = &point0L;
				ppoint1 = &point1L;
				cColor = color_rgba(0, 255, 0, 255);
				sColor = color_rgba(128, 255, 128, 255);
				pM = &ML;
				pLastPos = &LastPosL;
			}
			break;
			case 1:
			{
				ppoint0 = &point0S;
				ppoint1 = &point1S;
				cColor = color_rgba(0, 0, 255, 255);
				sColor = color_rgba(128, 128, 255, 255);
				pM = &MS;
				pLastPos = &LastPosS;
			}
			break;
			case 2:
			{
				ppoint0 = &point0H;
				ppoint1 = &point1H;
				cColor = color_rgba(255, 0, 0, 255);
				sColor = color_rgba(255, 128, 128, 255);
				pM = &MH;
				pLastPos = &LastPosH;
			}
			break;
		}

		//drawing path trajectory
		f32 c = 0;
		for (s32 i = 0; i < 11; i++)
		{
			c = f32(i) * 0.1f;
			for (u32 k = 0; k < 3; k++)
			{
				point1S[k] = c * (c * (c * SCoeff[k][0] + SCoeff[k][1]) + SCoeff[k][2]) + SCoeff[k][3];
				point1H[k] = c * (c * (c * HCoeff[k][0] + HCoeff[k][1]) + HCoeff[k][2]) + HCoeff[k][3];
				point1L[k] = IStart.Pos[k] + c * (IEnd.Pos[k] - IStart.Pos[k]);
			}

			if (i != 0)
			{
				Level( ).debug_renderer( ).draw_line(*pM, *ppoint0, *ppoint1, cColor);
			}

			point0S.set(point1S);
			point0H.set(point1H);
			point0L.set(point1L);
		}

		//drawing speed vectors
		for (i = 0; i < 2; i++)
		{
			c = f32(i);
			for (u32 k = 0; k < 3; k++)
			{
				point1S[k] = c * (c * (c * SCoeff[k][0] + SCoeff[k][1]) + SCoeff[k][2]) + SCoeff[k][3];
				point1H[k] = c * (c * (c * HCoeff[k][0] + HCoeff[k][1]) + HCoeff[k][2]) + HCoeff[k][3];

				tS[k] = (c * c * SCoeff[k][0] * 3 + c * SCoeff[k][1] * 2 + SCoeff[k][2]) / 3; // �������� �� ������� � 3 ���� ��������� �������� ��� ������� ������������� !!!!
				tH[k] = (c * c * HCoeff[k][0] * 3 + c * HCoeff[k][1] * 2 + HCoeff[k][2]);
			}

			point0S.add(tS, point1S);
			point0H.add(tH, point1H);

			if (g_cl_InterpolationType > 0)
			{
				Level( ).debug_renderer( ).draw_line(*pM, *ppoint0, *ppoint1, sColor);
			}
		}

		//draw interpolation history curve
		if (!pLastPos->empty( ))
		{
			fVector3 Pos1;
			fVector3 Pos2;
			VIS_POSITION_it It = pLastPos->begin( );
			Pos1 = *It;
			for (; It != pLastPos->end( ); It++)
			{
				Pos2 = *It;

				Level( ).debug_renderer( ).draw_line(*pM, Pos1, Pos2, cColor);
				Level( ).debug_renderer( ).draw_aabb(Pos2, size / 5, size / 5, size / 5, sColor);
				Pos1 = *It;
			}
		}

		fVector3 PH;
		fVector3 PS;
		PH.set(IPosH); PH.y += 1;
		PS.set(IPosS); PS.y += 1;
		//		Level().debug_renderer().draw_aabb			(PS, size, size, size, color_rgba(128, 128, 255, 255));
		//		Level().debug_renderer().draw_aabb			(PH, size, size, size, color_rgba(255, 128, 128, 255));
				/////////////////////////////////////////////////////////////////////////////////
	}
	else
	{
		if (!(dbg_net_Draw_Flags.is_any((1 << 1))))
		{
			return;
		}

		CKinematics* V = smart_cast<CKinematics*>(Visual( ));
		if (dbg_net_Draw_Flags.test(1 << 0) && V)
		{
			u16 BoneCount = V->LL_BoneCount( );
			for (u16 i = 0; i < BoneCount; i++)
			{
				fObb BoneOBB = V->LL_GetBox(i);
				fMatrix4x4 BoneMatrix;
				BoneOBB.xform_get(BoneMatrix);
				fMatrix4x4 BoneMatrixRes;
				BoneMatrixRes.mul(V->LL_GetTransform(i), BoneMatrix);
				BoneMatrix.mul(XFORM( ), BoneMatrixRes);
				Level( ).debug_renderer( ).draw_obb(BoneMatrix, BoneOBB.m_halfsize, color_rgba(0, 255, 0, 255));
			}
		}

		if (!m_States.empty( ))
		{
			u32 NumBones = m_States.size( );
			for (u32 i = 0; i < NumBones; i++)
			{
				SPHNetState state = m_States[i];

				fVector3 half_dim;
				half_dim.x = 0.2f;
				half_dim.y = 0.1f;
				half_dim.z = 0.1f;

				u32 Color = color_rgba(255, 0, 0, 255);

				fMatrix4x4 M;

				M = Fidentity;
				M.rotation(state.quaternion);
				M.translate_add(state.position);
				Level( ).debug_renderer( ).draw_obb(M, half_dim, Color);

				if (!PHGetSyncItem(u16(i)))
				{
					continue;
				}

				PHGetSyncItem(u16(i))->get_State(state);

				Color = color_rgba(0, 255, 0, 255);
				M = Fidentity;
				M.rotation(state.quaternion);
				M.translate_add(state.position);
				Level( ).debug_renderer( ).draw_obb(M, half_dim, Color);
			}
		}
		else
		{
			if (!g_Alive( ) && PHGetSyncItemsNumber( ) > 2)
			{
				u16 NumBones = PHGetSyncItemsNumber( );
				for (u16 i = 0; i < NumBones; i++)
				{
					SPHNetState state;// = m_States[i];
					PHGetSyncItem(i)->get_State(state);

					fMatrix4x4 M;
					M = Fidentity;
					M.rotation(state.quaternion);
					M.translate_add(state.position);

					fVector3 half_dim;
					half_dim.x = 0.2f;
					half_dim.y = 0.1f;
					half_dim.z = 0.1f;

					u32 Color = color_rgba(0, 255, 0, 255);
					Level( ).debug_renderer( ).draw_obb(M, half_dim, Color);
				}
				//-----------------------------------------------------------------
				fVector3 min;
				fVector3 max;

				min.set(F_MAX, F_MAX, F_MAX);
				max.set(-F_MAX, -F_MAX, -F_MAX);
				/////////////////////////////////////
				for (u16 i = 0; i < NumBones; i++)
				{
					SPHNetState state;
					PHGetSyncItem(i)->get_State(state);

					fVector3& p = state.position;
					UpdateLimits(p, min, max);

					fVector3 px = state.linear_vel;
					px.div(10.0f);
					px.add(state.position);
					UpdateLimits(px, min, max);
				}

				CNetPacket PX;
				for (u16 i = 0; i < NumBones; i++)
				{
					SPHNetState state;
					PHGetSyncItem(i)->get_State(state);

					PX.B.count = 0;
					w_vec_q8(PX, state.position, min, max);
					w_qt_q8(PX, state.quaternion);
					//					w_vec_q8(PX,state.linear_vel,min,max);

					PX.r_pos = 0;
					r_vec_q8(PX, state.position, min, max);
					r_qt_q8(PX, state.quaternion);
					//					r_vec_q8(PX,state.linear_vel,min,max);
										//===============================================
					fMatrix4x4 M;
					M = Fidentity;
					M.rotation(state.quaternion);
					M.translate_add(state.position);

					fVector3 half_dim;
					half_dim.x = 0.2f;
					half_dim.y = 0.1f;
					half_dim.z = 0.1f;

					u32 Color = color_rgba(255, 0, 0, 255);
					Level( ).debug_renderer( ).draw_obb(M, half_dim, Color);
				}

				fVector3 LC;
				fVector3 LS;
				LC.add(min, max); LC.div(2.0f);
				LS.sub(max, min); LS.div(2.0f);

				Level( ).debug_renderer( ).draw_aabb(LC, LS.x, LS.y, LS.z, color_rgba(255, 128, 128, 255));
				//-----------------------------------------------------------------
			}
		}
	}
}
#endif

void CActor::net_Save(CNetPacket& P)
{

#ifdef DEBUG
	u32 pos;
	Msg("Actor net_Save");

	pos = P.w_tell( );
	inherited::net_Save(P);
	Msg("inherited::net_Save() : %d", P.w_tell( ) - pos);

	pos = P.w_tell( );
	m_pPhysics_support->in_NetSave(P);
	P.w_u16(m_holderID);
	Msg("m_pPhysics_support->in_NetSave() : %d", P.w_tell( ) - pos);
#else
	inherited::net_Save(P);
	m_pPhysics_support->in_NetSave(P);
	P.w_u16(m_holderID);
#endif

}

BOOL CActor::net_SaveRelevant( )
{
	return TRUE;
}

void CActor::Check_for_AutoPickUp( )
{
	if (!psActorFlags.test(AF_AUTOPICKUP))
	{
		return;
	}

	return;
}

void CActor::SetHitInfo(CObject* who, CObject* weapon, s16 element, fVector3 Pos, fVector3 Dir)
{
	m_iLastHitterID = (who != NULL) ? who->ID( ) : u16(-1);
	m_iLastHittingWeaponID = (weapon != NULL) ? weapon->ID( ) : u16(-1);
	m_s16LastHittedElement = element;
	m_fLastHealth = GetfHealth( );
	m_bWasHitted = true;
	m_vLastHitDir = Dir;
	m_vLastHitPos = Pos;
}

void CActor::OnPlayHeadShotParticle(CNetPacket P)
{
	fVector3	HitDir;
	fVector3 HitPos;
	s16	element = P.r_s16( );
	P.r_dir(HitDir);	HitDir.invert( );
	P.r_vec3(HitPos);
	//-----------------------------------
	if (!m_sHeadShotParticle.size( ))
	{
		return;
	}

	fMatrix4x4 pos;
	CParticlesPlayer::MakeXFORM(this, element, HitDir, HitPos, pos);
	// ���������� particles
	CParticlesObject* ps = NULL;

	ps = CParticlesObject::Create(m_sHeadShotParticle.c_str( ), TRUE);

	ps->UpdateParent(pos, fVector3( ).set(0.0f, 0.0f, 0.0f));
	GamePersistent( ).ps_needtoplay.push_back(ps);
}

bool CActor::Check_for_BackStab_Bone(u16 element)
{
	if (element == m_head)
	{
		return true;
	}
	else if (element == m_neck)
	{
		return true;
	}
	else if (element == m_spine2)
	{
		return true;
	}
	else if (element == m_l_clavicle)
	{
		return true;
	}
	else if (element == m_r_clavicle)
	{
		return true;
	}
	else if (element == m_spine1)
	{
		return true;
	}
	else if (element == m_spine)
	{
		return true;
	}

	return false;
}

bool CActor::InventoryAllowSprint( )
{
	PIItem pActiveItem = inventory( ).ActiveItem( );
	if (pActiveItem && !pActiveItem->IsSprintAllowed( ))
	{
		return false;
	}

	PIItem pOutfitItem = inventory( ).ItemFromSlot(OUTFIT_SLOT);
	if (pOutfitItem && !pOutfitItem->IsSprintAllowed( ))
	{
		return false;
	}

	return true;
}

BOOL CActor::BonePassBullet(s32 boneID)
{
	return inherited::BonePassBullet(boneID);
}

void CActor::On_B_NotCurrentEntity( )
{
	inventory( ).Items_SetCurrentEntityHud(false);
}
