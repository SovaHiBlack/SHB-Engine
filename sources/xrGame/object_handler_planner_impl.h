////////////////////////////////////////////////////////////////////////////
//	Module 		: object_handler_planner_impl.h
//	Created 	: 11.03.2004
//  Modified 	: 01.12.2004
//	Author		: Dmitriy Iassenev
//	Description : Object handler action planner inline functions implementation
////////////////////////////////////////////////////////////////////////////

#pragma once

inline	CObjectHandlerPlanner::_condition_type CObjectHandlerPlanner::uid(const u32 id0, const u32 id1) const
{
	VERIFY				(!((id0 << 16) & id1));
	return				((id0 << 16) | id1);
}

inline	bool CObjectHandlerPlanner::object_action	(u32 action_id, CObject *object)
{
	return				((action_id >> 16) == object->ID());
}

inline	u32	CObjectHandlerPlanner::current_action_object_id	() const
{
	return				(action_object_id(current_action_id()));
}

inline	u32	CObjectHandlerPlanner::action_object_id		(_condition_type action_id) const
{
	return				(action_id >> 16);
}

inline	void CObjectHandlerPlanner::add_condition			(CActionBase<CStalker> *action, u16 id, ObjectHandlerSpace::EWorldProperties property, _value_type value)
{
	action->add_condition(CWorldProperty(uid(id,property),value));
}

inline	void CObjectHandlerPlanner::add_effect				(CActionBase<CStalker> *action, u16 id, ObjectHandlerSpace::EWorldProperties property, _value_type value)
{
	action->add_effect	(CWorldProperty(uid(id,property),value));
}
