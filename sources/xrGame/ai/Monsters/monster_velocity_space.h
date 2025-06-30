#pragma once

namespace MonsterMovement
{
	enum EMovementParameters
	{
		eMP_IDLE = u32(1) << 1,
		eMP_STAND = u32(1) << 4,
		eVelocityParameterWalkNormal = u32(1) << 3,
		eVelocityParameterRunNormal = u32(1) << 2,

		eVelocityParameterWalkDamaged = u32(1) << 5,
		eVelocityParameterRunDamaged = u32(1) << 6,
		eVelocityParameterSteal = u32(1) << 7,
		eVelocityParameterDrag = u32(1) << 8,
		eVelocityParameterInvisible = u32(1) << 9,
		eVelocityParameterRunAttack = u32(1) << 10,

		eVelocityParamsWalk = eMP_STAND | eVelocityParameterWalkNormal,
		eVelocityParamsWalkDamaged = eMP_STAND | eVelocityParameterWalkDamaged,
		eVelocityParamsRun = eMP_STAND | eVelocityParameterWalkNormal | eVelocityParameterRunNormal,
		eVelocityParamsRunDamaged = eMP_STAND | eVelocityParameterWalkDamaged | eVelocityParameterRunDamaged,
		eVelocityParamsAttackNorm = eMP_STAND | eVelocityParameterWalkNormal | eVelocityParameterRunNormal,
		eVelocityParamsAttackDamaged = eMP_STAND | eVelocityParameterWalkDamaged | eVelocityParameterRunDamaged,
		eVelocityParamsSteal = eMP_STAND | eVelocityParameterSteal,
		eVelocityParamsDrag = eMP_STAND | eVelocityParameterDrag,
		eVelocityParamsInvisible = eVelocityParameterInvisible | eMP_STAND,
		eVelocityParamsRunAttack = eVelocityParameterRunAttack | eMP_STAND,

		eVelocityParameterCustom = u32(1) << 12
	};

	enum EMovementParametersChimera
	{
		eChimeraVelocityParameterUpperWalkFwd = eVelocityParameterCustom << 1,
		eChimeraVelocityParameterJumpGround = eVelocityParameterCustom << 2,

		eChimeraVelocityParamsUpperWalkFwd = eMP_STAND | eChimeraVelocityParameterUpperWalkFwd,
	};

	enum EMovementParametersSnork
	{
		eSnorkVelocityParameterJumpGround = eVelocityParameterCustom << 2,
	};

	enum EMovementParametersController
	{
		eControllerVelocityParameterMoveFwd = eVelocityParameterCustom << 1,
		eControllerVelocityParameterMoveBkwd = eVelocityParameterCustom << 2,

		eControllerVelocityParamsMoveFwd = eControllerVelocityParameterMoveFwd | eMP_STAND,
		eControllerVelocityParamsMoveBkwd = eControllerVelocityParameterMoveBkwd | eMP_STAND,
	};

	enum EMovementParametersGiant
	{
		eGiantVelocityParameterJumpPrepare = eVelocityParameterCustom << 1,
		eGiantVelocityParameterJumpGround = eVelocityParameterCustom << 2,
	};
};
