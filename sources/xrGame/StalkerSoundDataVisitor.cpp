////////////////////////////////////////////////////////////////////////////
//	Module 		: StalkerSoundDataVisitor.cpp
//	Description : Stalker sound data visitor
////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include "StalkerSoundDataVisitor.h"
#include "ai/stalker/Stalker.h"
#include "StalkerSoundData.h"
#include "agent_manager.h"
#include "agent_member_manager.h"
#include "memory_manager.h"
#include "hit_memory_manager.h"
#include "visual_memory_manager.h"
#include "enemy_manager.h"
#include "danger_manager.h"

CStalkerSoundDataVisitor::~CStalkerSoundDataVisitor( )
{ }

void CStalkerSoundDataVisitor::visit(CStalkerSoundData* data)
{
	if (object( ).memory( ).enemy( ).selected( ))
	{
		return;
	}

	if (object( ).is_relation_enemy(&data->object( )))
	{
		return;
	}

	if (!data->object( ).memory( ).enemy( ).selected( ))
	{
		if (!object( ).memory( ).danger( ).selected( ) && data->object( ).memory( ).danger( ).selected( ))
		{
			object( ).memory( ).danger( ).add(*data->object( ).memory( ).danger( ).selected( ));
		}

		return;
	}

	if (data->object( ).memory( ).enemy( ).selected( )->getDestroy( ))
	{
		return;
	}

	if (!object( ).is_relation_enemy(data->object( ).memory( ).enemy( ).selected( )))
	{
		return;
	}

	if (!data->object( ).g_Alive( ))
	{
		return;
	}

	if (!object( ).g_Alive( ))
	{
		return;
	}

	Msg("%s : Adding fiction hit by sound info from stalker %s", *object( ).cName( ), *data->object( ).cName( ));

	object( ).memory( ).make_object_visible_somewhen(data->object( ).memory( ).enemy( ).selected( ));

//	const MemorySpace::CHitObject	*m = data->object().memory().hit().hit(data->object().memory().enemy().selected());
//	if (!m)
//		return;
//	object().memory().hit().add		(*m);
}
