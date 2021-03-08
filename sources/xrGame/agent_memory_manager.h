////////////////////////////////////////////////////////////////////////////
//	Module 		: agent_memory_manager_inline.h
//	Created 	: 24.05.2004
//  Modified 	: 14.01.2005
//	Author		: Dmitriy Iassenev
//	Description : Agent memory manager inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "memory_space.h"

class CAgentManager;

class CAgentMemoryManager {
public:
	typedef MemorySpace::CVisibleObject		CVisibleObject;
	typedef MemorySpace::CSoundObject		CSoundObject;
	typedef MemorySpace::CHitObject			CHitObject;

public:
	typedef xr_vector<CVisibleObject>		VISIBLES;
	typedef xr_vector<CSoundObject>			SOUNDS;
	typedef xr_vector<CHitObject>			HITS;
	typedef MemorySpace::squad_mask_type	squad_mask_type;


protected:
	CAgentManager			*m_object;
	VISIBLES				*m_visible_objects;
	SOUNDS					*m_sound_objects;
	HITS					*m_hit_objects;

protected:
	inline		CAgentManager	&object				() const;

public:
	inline						CAgentMemoryManager	(CAgentManager *object);
			void			update				();
			void			remove_links		(CObject *object);

public:
	inline		void			set_squad_objects	(VISIBLES *objects);
	inline		void			set_squad_objects	(SOUNDS *objects);
	inline		void			set_squad_objects	(HITS *objects);

public:
	inline		VISIBLES		&visibles			() const;
	inline		SOUNDS			&sounds				() const;
	inline		HITS			&hits				() const;

public:
	template <typename T>
	inline		void			reset_memory_masks	(T &objects);
			void			reset_memory_masks	();

	template <typename T>
	inline		void			update_memory_masks	(const squad_mask_type &mask, T &objects);
	inline		void			update_memory_mask	(const squad_mask_type &mask, squad_mask_type &current);
			void			update_memory_masks	(const squad_mask_type &mask);
			void			object_information	(const CObject *object, u32 &level_time, Fvector3& position);
};

#include "agent_memory_manager_inline.h"
