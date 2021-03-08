////////////////////////////////////////////////////////////////////////////
//	Module 		: control_action_inline.h
//	Created 	: 05.04.2004
//  Modified 	: 05.04.2004
//	Author		: Dmitriy Iassenev
//	Description : Control action
////////////////////////////////////////////////////////////////////////////

#pragma once

inline	CControlAction::CControlAction		()
{
}

inline	void CControlAction::set_object		(CStalker *object)
{
	VERIFY				(object);
	m_object			= object;
}

inline	bool CControlAction::applicable		() const
{
	return				(true);
}

inline	bool CControlAction::completed		() const
{
	return				(true);
}

inline	void CControlAction::initialize		()
{
}

inline	void CControlAction::execute		()
{
}

inline	void CControlAction::finalize		()
{
}

inline	CStalker &CControlAction::object	() const
{
	VERIFY				(m_object);
	return				(*m_object);
}

inline	void CControlAction::remove_links	(CObject *object)
{
}
