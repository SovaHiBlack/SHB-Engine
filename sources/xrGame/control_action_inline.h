////////////////////////////////////////////////////////////////////////////
//	Module 		: control_action_inline.h
//	Created 	: 05.04.2004
//  Modified 	: 05.04.2004
//	Author		: Dmitriy Iassenev
//	Description : Control action
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	CControlAction::CControlAction		()
{
}

IC	void CControlAction::set_object(CStalker* object)
{
	VERIFY				(object);
	m_object			= object;
}

IC	bool CControlAction::applicable		() const
{
	return				(true);
}

IC	bool CControlAction::completed		() const
{
	return				(true);
}

IC	void CControlAction::initialize		()
{
}

IC	void CControlAction::execute		()
{
}

IC	void CControlAction::finalize		()
{
}

IC	CStalker&CControlAction::object	() const
{
	VERIFY				(m_object);
	return				(*m_object);
}

IC	void CControlAction::remove_links	(CObject *object)
{
}
