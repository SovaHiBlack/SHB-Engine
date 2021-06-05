#pragma once

#include "associative_vector.h"

#define MIN_LEGS_COUNT	1
#define MAX_LEGS_COUNT	4

struct MotionID;

struct S_StepParam
{
	struct
	{
		float	time;
		float	power;
	} step[MAX_LEGS_COUNT];

	u8			cycles;
};

using StepsMap = associative_vector<MotionID, S_StepParam>;
using StepsMap_it = StepsMap::iterator;

struct S_StepInfo
{
	struct
	{
		bool			handled;		// обработан
		u8				cycle;			// цикл в котором отработан
	} activity[MAX_LEGS_COUNT];

	S_StepParam		params;
	bool			disable;

	u8				cur_cycle;

	S_StepInfo		( )
	{
		disable = true;
	}
};

enum E_LegType
{
	eLT_FrontLeft,
	eLT_FrontRight,
	eLT_BackRight,
	eLT_BackLeft
};
