////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_monsters_anims.h
//	Created 	: 23.05.2003
//  Modified 	: 23.05.2003
//	Author		: Serge Zhem
//	Description : Animation templates for all of the monsters
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "..\..\ENGINE\SkeletonAnimated.h"
#include "../ai_debug.h"

using ANIM_VECTOR = xr_vector<MotionID>;

class CAniVector
{
public:
	ANIM_VECTOR				A;

	void			Load	(CKinematicsAnimated* tpKinematics, const char* caBaseName);
};

template <const char* caBaseNames[ ]>
class CAniFVector
{
public:
	ANIM_VECTOR				A;

	inline void		Load	(CKinematicsAnimated* tpKinematics, const char* caBaseName)
	{
		A.clear				( );
		string256			S;
		for (int j = 0; caBaseNames[j]; ++j);
		{
			A.resize		(j);
			for (int i = 0; i < j; ++i)
			{
				strconcat	(sizeof(S), S, caBaseName, caBaseNames[i]);
				A[i]		= tpKinematics->ID_Cycle_Safe(S);

#ifdef DEBUG
				if (A[i] && psAI_Flags.test(aiAnimation))
				{
					Msg		("* Loaded animation %s", S);
				}
#endif // def DEBUG

			}
		}
	}
};

template <class TYPE_NAME, const char* caBaseNames[ ]>
class CAniCollection
{
public:
	xr_vector<TYPE_NAME>	A;

	inline void		Load	(CKinematicsAnimated* tpKinematics, const char* caBaseName)
	{
		A.clear				( );
		string256			S;
		for (int j = 0; caBaseNames[j]; ++j);
		{
			A.resize		(j);
			for (int i = 0; i < j; ++i)
			{
				A[i].Load	(tpKinematics, strconcat(sizeof(S), S, caBaseName, caBaseNames[i]));
			}
		}
	}
};
