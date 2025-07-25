////////////////////////////////////////////////////////////////////////////
//	Module 		: memory_manager.h
//	Created 	: 02.10.2001
//  Modified 	: 19.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Memory manager
////////////////////////////////////////////////////////////////////////////

#pragma once

class CVisualMemoryManager;
class CSoundMemoryManager;
class CHitMemoryManager;
class CEnemyManager;
class CItemManager;
class CDangerManager;
class CCustomMonster;
class CStalker;
class CEntityAlive;
class CSound_UserDataVisitor;

namespace MemorySpace
{
	struct SMemoryInfo;
};

class CMemoryManager
{
public:
	typedef MemorySpace::SMemoryInfo						MemoryInfo;

protected:
	CVisualMemoryManager* m_visual;
	CSoundMemoryManager* m_sound;
	CHitMemoryManager* m_hit;
	CEnemyManager* m_enemy;
	CItemManager* m_item;
	CDangerManager* m_danger;

protected:
	CCustomMonster* m_object;
	CStalker* m_stalker;

private:
	void				update_enemies(const bool& registered_in_combat);

protected:
	template <typename T>
	void				update(const xr_vector<T>& objects, bool add_enemies);

public:
	CMemoryManager(CEntityAlive* entity_alive, CSound_UserDataVisitor* visitor);
	virtual						~CMemoryManager( );
	virtual	void				Load(pcstr section);
	virtual	void				reinit( );
	virtual	void				reload(pcstr section);
	virtual	void				update(f32 time_delta);
	void				remove_links(CObject* object);
	virtual void				on_restrictions_change( );

public:
	void				enable(const CObject* object, bool enable);
	MemoryInfo			memory(const CObject* object) const;
	u32					memory_time(const CObject* object) const;
	fVector3				memory_position(const CObject* object) const;
	void				make_object_visible_somewhen(const CEntityAlive* enemy);

public:
	template <typename T, typename _predicate>
	IC		void				fill_enemies(const xr_vector<T>& objects, const _predicate& predicate) const;
	template <typename _predicate>
	IC		void				fill_enemies(const _predicate& predicate) const;

public:
	IC		CVisualMemoryManager& visual( ) const;
	IC		CSoundMemoryManager& sound( ) const;
	IC		CHitMemoryManager& hit( ) const;
	IC		CEnemyManager& enemy( ) const;
	IC		CItemManager& item( ) const;
	IC		CDangerManager& danger( ) const;
	IC		CCustomMonster& object( ) const;
	IC		CStalker& stalker( ) const;

public:
	void				save(CNetPacket& packet) const;
	void				load(IReader& packet);
	void xr_stdcall		on_requested_spawn(CObject* object);
};

#include "memory_manager_inline.h"