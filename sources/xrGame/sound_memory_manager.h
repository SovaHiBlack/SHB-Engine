////////////////////////////////////////////////////////////////////////////
//	Module 		: sound_memory_manager.h
//	Created 	: 02.10.2001
//  Modified 	: 19.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Sound memory manager
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "memory_space.h"

#ifdef DEBUG
#	define USE_SELECTED_SOUND
#endif

namespace MemorySpace {
	struct CSoundObject;
};

enum ESoundTypes;

class CCustomMonster;
class CStalker;

class CSoundMemoryManager {
public:
	typedef MemorySpace::CSoundObject					CSoundObject;
	typedef xr_vector<CSoundObject>						SOUNDS;
	typedef xr_map<ESoundTypes,u32>						PRIORITIES;

private:
	struct CDelayedSoundObject {
		ALife::_OBJECT_ID	m_object_id;
		CSoundObject		m_sound_object;
	};

private:
	typedef xr_vector<CDelayedSoundObject>				DELAYED_SOUND_OBJECTS;

private:
	CCustomMonster				*m_object;
	CStalker* m_stalker;
	CSound_UserDataVisitor		*m_visitor;
	DELAYED_SOUND_OBJECTS		m_delayed_objects;

private:
	SOUNDS						*m_sounds;
	PRIORITIES					m_priorities;
	u32							m_max_sound_count;
	
private:
	u32							m_last_sound_time;
	u32							m_sound_decrease_quant;
	f32						m_decrease_factor;
	f32						m_min_sound_threshold;
	f32						m_sound_threshold;
	f32						m_self_sound_factor;

private:
	f32						m_weapon_factor;
	f32						m_item_factor;
	f32						m_npc_factor;
	f32						m_anomaly_factor;
	f32						m_world_factor;

private:
#ifdef USE_SELECTED_SOUND
	CSoundObject				*m_selected_sound;
#endif

private:
	IC		void				update_sound_threshold	();
	IC		u32					priority				(const CSoundObject &sound) const;
			void				add						(const CSoundObject &sound_object, bool check_for_existance = false);
			void				add						(const CObject *object, int sound_type, const fVector3& position, f32 sound_power);

protected:
	IC		void				priority				(const ESoundTypes &sound_type, u32 priority);

public:
	IC							CSoundMemoryManager(CCustomMonster* object, CStalker* stalker, CSound_UserDataVisitor* visitor);
	virtual						~CSoundMemoryManager	();
	virtual	void				Load					(pcstr section);
	virtual	void				reinit					();
	virtual	void				reload					(pcstr section);
	virtual void				feel_sound_new			(CObject* who, int eType, CSound_UserDataPtr user_data, const fVector3& Position, f32 power);
	virtual	void				update					();
			void				remove_links			(CObject *object);

public:
			void				enable					(const CObject *object, bool enable);

public:
	IC		const SOUNDS		&objects				() const;
#ifdef USE_SELECTED_SOUND
	IC		const CSoundObject	*sound					() const;
#endif
	IC		void				set_squad_objects		(SOUNDS *squad_objects);

public:
	IC		void				set_threshold			(f32 threshold);
	IC		void				restore_threshold		();

public:
			void				save					(CNetPacket& packet) const;
			void				load					(IReader &packet);
			void				on_requested_spawn		(CObject *object);

private:
			void				clear_delayed_objects	();
};

#include "sound_memory_manager_inline.h"