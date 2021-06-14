////////////////////////////////////////////////////////////////////////////
//	Module 		: MaterialManager_inline.h
//	Created 	: 27.12.2003
//  Modified 	: 27.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Material manager inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

inline	U16	CMaterialManager::last_material_idx		() const
{
	return				(m_last_material_idx);
}

inline	U16	CMaterialManager::self_material_idx		() const
{
	return				(m_my_material_idx);
}

inline SGameMtlPair	*CMaterialManager::get_current_pair()
{
	return GMLib.GetMaterialPair(m_my_material_idx,m_last_material_idx);
}
