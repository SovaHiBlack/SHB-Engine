////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_danger_unknown_planner.cpp
//	Created 	: 31.05.2005
//  Modified 	: 31.05.2005
//	Author		: Dmitriy Iassenev
//	Description : Stalker danger unknown planner class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "stalker_danger_unknown_planner.h"
#include "ai/stalker/Stalker.h"
#include "ai/stalker/ai_stalker_space.h"
#include "script_game_object.h"
#include "stalker_danger_property_evaluators.h"
#include "agent_manager.h"
#include "agent_member_manager.h"
#include "stalker_danger_unknown_actions.h"

using namespace StalkerDecisionSpace;

CStalkerDangerUnknownPlanner::CStalkerDangerUnknownPlanner(CStalker* object, pcstr action_name) :
	inherited(object, action_name)
{ }

void CStalkerDangerUnknownPlanner::setup(CStalker* object, CPropertyStorage* storage)
{
	inherited::setup(object, storage);
	clear( );
	add_evaluators( );
	add_actions( );
}

void CStalkerDangerUnknownPlanner::initialize( )
{
	inherited::initialize( );

	object( ).agent_manager( ).member( ).member(&object( )).cover(0);

	CScriptActionPlanner::m_storage.set_property(eWorldPropertyCoverReached, false);
	CScriptActionPlanner::m_storage.set_property(eWorldPropertyLookedAround, false);
}

void CStalkerDangerUnknownPlanner::update( )
{
	inherited::update( );
}

void CStalkerDangerUnknownPlanner::finalize( )
{
	inherited::finalize( );
}

void CStalkerDangerUnknownPlanner::add_evaluators( )
{
	add_evaluator(eWorldPropertyDanger, xr_new<CStalkerPropertyEvaluatorDangers>(m_object, "danger"));
	add_evaluator(eWorldPropertyCoverActual, xr_new<CStalkerPropertyEvaluatorDangerUnknownCoverActual>(m_object, "danger unknown : cover actual"));
	add_evaluator(eWorldPropertyCoverReached, xr_new<CStalkerPropertyEvaluatorMember>((CPropertyStorage*)0, eWorldPropertyCoverReached, true, true, "danger unknown : cover reached"));
	add_evaluator(eWorldPropertyLookedAround, xr_new<CStalkerPropertyEvaluatorMember>((CPropertyStorage*)0, eWorldPropertyLookedAround, true, true, "danger unknown : looked around"));
}

void CStalkerDangerUnknownPlanner::add_actions( )
{
	CStalkerActionBase* action;

	action = xr_new<CStalkerActionDangerUnknownTakeCover>(m_object, "take cover");
	add_effect(action, eWorldPropertyCoverActual, true);
	add_effect(action, eWorldPropertyCoverReached, true);
	add_operator(eWorldOperatorDangerUnknownTakeCover, action);

	action = xr_new<CStalkerActionDangerUnknownLookAround>(m_object, "look around");
	add_condition(action, eWorldPropertyCoverActual, true);
	add_condition(action, eWorldPropertyCoverReached, true);
	add_condition(action, eWorldPropertyLookedAround, false);
	add_effect(action, eWorldPropertyLookedAround, true);
	add_operator(eWorldOperatorDangerUnknownLookAround, action);

	action = xr_new<CStalkerActionDangerUnknownSearch>(m_object, "search");
	add_condition(action, eWorldPropertyCoverActual, true);
	add_condition(action, eWorldPropertyCoverReached, true);
	add_condition(action, eWorldPropertyLookedAround, true);
	add_effect(action, eWorldPropertyDanger, false);
	add_operator(eWorldOperatorDangerUnknownSearchEnemy, action);
}
