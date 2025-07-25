////////////////////////////////////////////////////////////////////////////
//	Module 		: visual_memory_manager.h
//	Created 	: 02.10.2001
//  Modified 	: 19.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Visual memory manager
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "visual_memory_params.h"
#include "memory_space.h"

class CCustomMonster;
class CStalker;
class CVisionClient;

class CVisualMemoryManager
{
#ifdef DEBUG
	friend class CStalker;
#endif
public:
	typedef MemorySpace::CVisibleObject			CVisibleObject;
	typedef MemorySpace::CNotYetVisibleObject	CNotYetVisibleObject;
	typedef xr_vector<CVisibleObject>			VISIBLES;
	typedef xr_vector<CObject*>					RAW_VISIBLES;
	typedef xr_vector<CNotYetVisibleObject>		NOT_YET_VISIBLES;

private:
	struct CDelayedVisibleObject
	{
		ALife::_OBJECT_ID	m_object_id;
		CVisibleObject		m_visible_object;
	};

private:
	typedef xr_vector<CDelayedVisibleObject>	DELAYED_VISIBLE_OBJECTS;

private:
	CCustomMonster* m_object;
	CStalker* m_stalker;
	CVisionClient* m_client;

private:
	RAW_VISIBLES		m_visible_objects;
	VISIBLES* m_objects;
	NOT_YET_VISIBLES	m_not_yet_visible_objects;

private:
	DELAYED_VISIBLE_OBJECTS	m_delayed_objects;

private:
	CVisionParameters	m_free;
	CVisionParameters	m_danger;

private:
	u32					m_max_object_count;
	bool				m_enabled;
	u32					m_last_update_time;

public:
	void	add_visible_object(const CObject* object, f32 time_delta, bool fictitious = false);

protected:
	IC		void	fill_object(CVisibleObject& visible_object, const CGameObject* game_object);
	void	add_visible_object(const CVisibleObject visible_object);
	f32	object_visible_distance(const CGameObject* game_object, f32& object_distance) const;
	f32	object_luminocity(const CGameObject* game_object) const;
	f32	get_visible_value(f32 distance, f32 object_distance, f32 time_delta, f32 object_velocity, f32 luminocity) const;
	f32	get_object_velocity(const CGameObject* game_object, const CNotYetVisibleObject& not_yet_visible_object) const;
	u32		get_prev_time(const CGameObject* game_object) const;

public:
	u32		visible_object_time_last_seen(const CObject* object) const;

protected:
	void	add_not_yet_visible_object(const CNotYetVisibleObject& not_yet_visible_object);
	CNotYetVisibleObject* not_yet_visible_object(const CGameObject* game_object);

private:
	void	initialize( );

public:
	CVisualMemoryManager(CCustomMonster* object);
	CVisualMemoryManager(CStalker* stalker);
	CVisualMemoryManager(CVisionClient* client);
	virtual			~CVisualMemoryManager( );
	virtual	void	reinit( );
	virtual	void	reload(pcstr section);
	virtual	void	update(f32 time_delta);
	virtual	f32	feel_vision_mtl_transp(CObject* O, u32 element);
	void	remove_links(CObject* object);

public:
	bool	visible(const CGameObject* game_object, f32 time_delta);
	bool	visible(u32 level_vertex_id, f32 yaw, f32 eye_fov) const;

public:
	IC		void	set_squad_objects(xr_vector<CVisibleObject>* squad_objects);
	CVisibleObject* visible_object(const CGameObject* game_object);

public:
			// this function returns true if and only if 
			// specified object is visible now
	bool	visible_right_now(const CGameObject* game_object) const;
	// if current_params.m_still_visible_time == 0
	// this function returns true if and only if 
	// specified object is visible now
	// if current_params.m_still_visible_time > 0
	// this function returns true if and only if 
	// specified object is visible now or 
	// some time ago <= current_params.m_still_visible_time
	bool	visible_now(const CGameObject* game_object) const;

public:
	void	enable(const CObject* object, bool enable);

public:
	IC		f32	visibility_threshold( ) const;
	IC		f32	transparency_threshold( ) const;

public:
	IC		bool	enabled( ) const;
	IC		void	enable(bool value);

public:
	IC		const VISIBLES& objects( ) const;
	IC		const RAW_VISIBLES& raw_objects( ) const;
	IC		const NOT_YET_VISIBLES& not_yet_visible_objects( ) const;
	IC		const CVisionParameters& current_state( ) const;
	IC		squad_mask_type			mask( ) const;

public:
#ifdef DEBUG
	void					check_visibles( ) const;
#endif

public:
	void					save(CNetPacket& packet) const;
	void					load(IReader& packet);
	void					on_requested_spawn(CObject* object);

private:
	void					clear_delayed_objects( );
};

#include "visual_memory_manager_inline.h"