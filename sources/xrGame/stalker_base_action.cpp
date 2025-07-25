////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_base_action.cpp
//	Created 	: 25.03.2004
//  Modified 	: 27.09.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker base action
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "stalker_base_action.h"
#include "ai/stalker/Stalker.h"
#include "script_game_object.h"
#include "stalker_animation_manager.h"
#include "stalker_planner.h"
#include "ai/stalker/ai_stalker_space.h"

using namespace StalkerSpace;

CStalkerActionBase::CStalkerActionBase(CStalker* object, pcstr action_name) : inherited(object, action_name)
{ }

void CStalkerActionBase::initialize( )
{
	inherited::initialize( );
	object( ).animation( ).clear_script_animations( );
	object( ).brain( ).affect_cover(false);
}

void CStalkerActionBase::execute( )
{
	inherited::execute( );
}

void CStalkerActionBase::finalize( )
{
	inherited::finalize( );
	object( ).animation( ).clear_script_animations( );
}
