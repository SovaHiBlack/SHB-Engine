////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_monsters_misc.cpp
//	Created 	: 23.07.2002
//  Modified 	: 23.07.2002
//	Author		: Dmitriy Iassenev
//	Description : Miscellanious routines for monsters
////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include "ai_monsters_misc.h"
#include "../AISpace.h"
#include "../custommonster.h"
#include "../ef_storage.h"
#include "../SeniorityHierarchyHolder.h"
#include "../TeamHierarchyHolder.h"
#include "../SquadHierarchyHolder.h"
#include "../GroupHierarchyHolder.h"
#include "..\..\XR_3DA\skeletonanimated.h"
#include "ai_monsters_anims.h"
#include "../ef_pattern.h"
#include "../memory_manager.h"
#include "../enemy_manager.h"
#include "../Level.h"
#include "../agent_manager.h"
#include "../agent_member_manager.h"
#include "stalker/Stalker.h"

bool bfGetActionSuccessProbability(GroupHierarchyHolder::MEMBER_REGISTRY& Members, const xr_vector<const CEntityAlive*>& VisibleEnemies, f32 fMinProbability, CBaseFunction& fSuccessProbabilityFunction)
{
	s32 i = 0;
	s32 j = 0;
	s32 I = (s32)Members.size( );
	s32 J = (s32)VisibleEnemies.size( );
	xr_vector<const CEntityAlive*>::const_iterator	II = VisibleEnemies.begin( );
	for (; (i < I) && (j < J); )
	{
		ai( ).ef_storage( ).non_alife( ).member( ) = smart_cast<CEntityAlive*>(Members[i]);
		if (!(ai( ).ef_storage( ).non_alife( ).member( )) || !(ai( ).ef_storage( ).non_alife( ).member( )->g_Alive( )))
		{
			++i;
			continue;
		}
		ai( ).ef_storage( ).non_alife( ).enemy( ) = *II;
		if (!(ai( ).ef_storage( ).non_alife( ).enemy( )) || !(ai( ).ef_storage( ).non_alife( ).enemy( )->g_Alive( )))
		{
			++j;
			++II;
			continue;
		}
		f32 fProbability = fSuccessProbabilityFunction.ffGetValue( ) / 100.f, fCurrentProbability;
		if (fProbability > fMinProbability)
		{
			fCurrentProbability = fProbability;
			for (++j; (i < I) && (j < J); ++j)
			{
				ai( ).ef_storage( ).non_alife( ).enemy( ) = *II;
				if (!(ai( ).ef_storage( ).non_alife( ).enemy( )) || !(ai( ).ef_storage( ).non_alife( ).enemy( )->g_Alive( )))
				{
					++j;
					++II;
					continue;
				}
				fProbability = fSuccessProbabilityFunction.ffGetValue( ) / 100.f;
				if (fCurrentProbability * fProbability < fMinProbability)
				{
					++i;
					break;
				}
				else
					fCurrentProbability *= fProbability;
			}
		}
		else
		{
			fCurrentProbability = 1.0f - fProbability;
			for (++i; (i < I) && (j < J); ++i)
			{
				ai( ).ef_storage( ).non_alife( ).member( ) = smart_cast<CEntityAlive*>(Members[i]);
				if (!(ai( ).ef_storage( ).non_alife( ).member( )) || !(ai( ).ef_storage( ).non_alife( ).member( )->g_Alive( )))
				{
					++i;
					continue;
				}
				fProbability = 1.0f - fSuccessProbabilityFunction.ffGetValue( ) / 100.f;
				if (fCurrentProbability * fProbability < (1.f - fMinProbability))
				{
					++II;
					++j;
					break;
				}
				else
					fCurrentProbability *= fProbability;
			}
		}
	}
	return(j >= J);
}

u32 dwfChooseAction(u32 dwActionRefreshRate, f32 fMinProbability0, f32 fMinProbability1, f32 fMinProbability2, f32 fMinProbability3, u32 dwTeam, u32 dwSquad, u32 dwGroup, u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, CEntity* tpEntity, f32 fGroupDistance)
{
//	return(a0);
	CGroupHierarchyHolder& Group = Level( ).seniority_holder( ).team(dwTeam).squad(dwSquad).group(dwGroup);

	if (Device.dwTimeGlobal - Group.m_dwLastActionTime < dwActionRefreshRate)
	{
		switch (Group.m_dwLastAction)
		{
			case 0: return(a0);
			case 1: return(a1);
			case 2: return(a2);
			case 3: return(a3);
			case 4: return(a4);
			default: return(a4);
		}
	}

	const CCustomMonster* monster = smart_cast<const CCustomMonster*>(tpEntity);
	VERIFY(monster);
	const CStalker* stalker = smart_cast<const CStalker*>(monster);
	const xr_vector<const CEntityAlive*>& VisibleEnemies = monster->memory( ).enemy( ).objects( );

	GroupHierarchyHolder::MEMBER_REGISTRY	Members;
	if (!tpEntity)
		for (s32 k = 0; k < (s32)Group.members( ).size( ); ++k)
		{
			if (Group.members( )[k]->g_Alive( ) && ((Group.members( )[k]->spatial.type & STYPE_VISIBLEFORAI) == STYPE_VISIBLEFORAI))
				Members.push_back(Group.members( )[k]);
		}
	else
		for (s32 k = 0; k < (s32)Group.members( ).size( ); ++k)
		{
			if (Group.members( )[k]->g_Alive( ) && ((Group.members( )[k]->spatial.type & STYPE_VISIBLEFORAI) == STYPE_VISIBLEFORAI))
				if (tpEntity->Position( ).distance_to(Group.members( )[k]->Position( )) < fGroupDistance)
				{

					if (!stalker)
					{
						Members.push_back(Group.members( )[k]);
						continue;
					}

					const CStalker* member = smart_cast<CStalker*>(Group.members( )[k]);
					if (!member)
					{
						Members.push_back(Group.members( )[k]);
						continue;
					}

					if (Group.agent_manager( ).member( ).registered_in_combat(member))
					{
						Members.push_back(Group.members( )[k]);
					}
					else if (member->ID( ) == tpEntity->ID( ))
					{
						Members.push_back(Group.members( )[k]);
					}
				}
		}

	ai( ).ef_storage( ).non_alife( ).member_item( ) = 0;
	ai( ).ef_storage( ).non_alife( ).enemy_item( ) = 0;

	WRITE_QUERY_TO_LOG("\nNew query");
	if (bfGetActionSuccessProbability(Members, VisibleEnemies, fMinProbability0, *ai( ).ef_storage( ).m_pfVictoryProbability))
	{
		Group.m_dwLastActionTime = Device.dwTimeGlobal;
		Group.m_dwLastAction = 0;
		WRITE_QUERY_TO_LOG("Attack");
		return						(a0);
	}
	else
		if (bfGetActionSuccessProbability(Members, VisibleEnemies, fMinProbability1, *ai( ).ef_storage( ).m_pfVictoryProbability))
		{
			Group.m_dwLastActionTime = Device.dwTimeGlobal;
			Group.m_dwLastAction = 1;
			WRITE_QUERY_TO_LOG("Attack 1");
			return						(a1);
		}
		else
			if (bfGetActionSuccessProbability(Members, VisibleEnemies, fMinProbability2, *ai( ).ef_storage( ).m_pfVictoryProbability))
			{
				Group.m_dwLastActionTime = Device.dwTimeGlobal;
				Group.m_dwLastAction = 2;
				WRITE_QUERY_TO_LOG("Defend");
				return						(a2);
			}
			else
				if (bfGetActionSuccessProbability(Members, VisibleEnemies, fMinProbability3, *ai( ).ef_storage( ).m_pfVictoryProbability))
				{
					Group.m_dwLastActionTime = Device.dwTimeGlobal;
					Group.m_dwLastAction = 3;
					WRITE_QUERY_TO_LOG("Defend 1");
					return						(a3);
				}
				else
				{
					Group.m_dwLastActionTime = Device.dwTimeGlobal;
					Group.m_dwLastAction = 4;
					WRITE_QUERY_TO_LOG("Retreat");
					return						(a4);
				}
}

void CAniVector::Load(CKinematicsAnimated* tpKinematics, pcstr caBaseName)
{
	A.clear( );
	string256	S1, S2;
	MotionID	tpMotionDef;
	for (s32 i = 0; ; ++i)
	{
		if (!!(tpMotionDef = tpKinematics->ID_Cycle_Safe(strconcat(sizeof(S1), S1, caBaseName, itoa(i, S2, 10)))))
		{
			A.push_back(tpMotionDef);

#ifdef DEBUG
			if (psAI_Flags.test(aiAnimation))
			{
				Msg("* Loaded animation %s", S1);
			}
#endif

		}
		else if (!!(tpMotionDef = tpKinematics->ID_FX_Safe(strconcat(sizeof(S1), S1, caBaseName, itoa(i, S2, 10)))))
		{
			A.push_back(tpMotionDef);

#ifdef DEBUG
			if (psAI_Flags.test(aiAnimation))
			{
				Msg("* Loaded animation fx %s", S1);
			}
#endif

		}
		else if (i < 10)
		{
			continue;
		}
		else
		{
			break;
		}
	}
}
