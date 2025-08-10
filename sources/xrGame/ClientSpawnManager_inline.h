////////////////////////////////////////////////////////////////////////////
//	Module 		: ClientSpawnManager_inline.h
//	Description : Seniority hierarchy holder inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC CClientSpawnManager::CClientSpawnManager( )
{ }

#ifdef DEBUG
IC const CClientSpawnManager::REQUEST_REGISTRY& CClientSpawnManager::registry( ) const
{
	return m_registry;
}
#endif // def DEBUG
