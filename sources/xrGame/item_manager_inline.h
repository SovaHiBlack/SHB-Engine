////////////////////////////////////////////////////////////////////////////
//	Module 		: item_manager_inline.h
//	Created 	: 27.12.2003
//  Modified 	: 27.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Item manager inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

inline	CItemManager::CItemManager						(CCustomMonster *object)
{
	VERIFY					(object);
	m_object				= object;
}
