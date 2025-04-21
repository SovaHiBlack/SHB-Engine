#pragma once

#include "PHSynchronize.h"
#include "xrserver_space.h"

#define ACTOR_HEIGHT		1.75f
#define ACTOR_LOOKOUT_ANGLE	PI_DIV_4
#define ACTOR_LOOKOUT_SPEED	2.f

namespace ACTOR_DEFS
{

	enum ESoundCcount
	{
		SND_DIE_COUNT = 4
	};

	enum EActorCameras
	{
		eacFirstEye = 0,
		eacLookAt,
		eacFreeLook,
		eacMaxCam
	};

	enum EDamages
	{
		DAMAGE_FX_COUNT = 12
	};

	enum EMoveCommand
	{
		mcFwd = (1ul << 0ul),
		mcBack = (1ul << 1ul),
		mcLStrafe = (1ul << 2ul),
		mcRStrafe = (1ul << 3ul),
		mcCrouch = (1ul << 4ul),
		mcAccel = (1ul << 5ul),
		mcTurn = (1ul << 6ul),
		mcJump = (1ul << 7ul),
		mcFall = (1ul << 8ul),
		mcLanding = (1ul << 9ul),
		mcLanding2 = (1ul << 10ul),
		mcClimb = (1ul << 11ul),
		mcSprint = (1ul << 12ul),
		mcLLookout = (1ul << 13ul),
		mcRLookout = (1ul << 14ul),
		mcAnyMove = (mcFwd | mcBack | mcLStrafe | mcRStrafe),
		mcAnyAction = (mcAnyMove | mcJump | mcFall | mcLanding | mcLanding2),
		mcAnyState = (mcCrouch | mcAccel | mcClimb | mcSprint),
		mcLookout = (mcLLookout | mcRLookout)
	};

	// enum дл€ определени€ действи€ над вещью на которую наведен в текущее врем€ прицел.
	// »спользуетс€ дл€ показа всплывающих динамических подсказок
	enum EActorAction
	{
		eaaNoAction = 0,
		eaaPickup,
		eaaTalk,
		eaaOpenDoor,
		eaaSearchCorpse
	};

	//---------------------------------------------
	// ввод с клавиатуры и мыши
	struct net_input
	{
		u32				m_dwTimeStamp;

		u32				mstate_wishful;

		u8				cam_mode;
		f32				cam_yaw;
		f32				cam_pitch;
		f32				cam_roll;

		bool operator < (const u32 Time)
		{
			return m_dwTimeStamp < Time;
		}
	};

	//------------------------------
	struct net_update
	{
		u32				dwTimeStamp;			// server(game) timestamp
		f32				o_model;				// model yaw
		SRotation		o_torso;				// torso in world coords
		fVector3		p_pos;					// in world coords
		fVector3		p_accel;				// in world coords
		fVector3		p_velocity;				// in world coords
		u32				mstate;
		s32				weapon;
		f32				fHealth;

		net_update( )
		{
			dwTimeStamp = 0;
			p_pos.set(0.0f, 0.0f, 0.0f);
			p_accel.set(0.0f, 0.0f, 0.0f);
			p_velocity.set(0.0f, 0.0f, 0.0f);
		}

		void lerp(net_update& A, net_update& B, f32 f);
	};

	///////////////////////////////////////////////////////
	// апдайт с данными физики
	struct net_update_A
	{
		u32					dwTimeStamp;
		SPHNetState			State;
	};

	///////////////////////////////////////////////////////
	// данные дл€ интерпол€ции
	struct InterpData
	{
		fVector3			Pos;
		fVector3			Vel;
		f32					o_model;				// model yaw
		SRotation			o_torso;				// torso in world coords
	};
};
