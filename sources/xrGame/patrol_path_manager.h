////////////////////////////////////////////////////////////////////////////
//	Module 		: patrol_path_manager.h
//	Created 	: 03.12.2003
//  Modified 	: 03.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Patrol path manager
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Level.h"
#include "script_engine.h"
#include "patrol_path_storage.h"
#include "patrol_path.h"
#include "patrol_path_manager_space.h"
#include "script_callback_ex.h"

template <typename _return_type>
class CScriptCallbackEx;

class CRestrictedObject;
class CGameObject;

using namespace PatrolPathManager;

class CPatrolPathManager {
private:
	friend struct CAccessabilityEvaluator;

private:
	typedef CScriptCallbackEx<bool>	CExtrapolateCallback;

private:
	const CPatrolPath				*m_path;
	CSharedString						m_path_name;
	EPatrolStartType				m_start_type;
	EPatrolRouteType				m_route_type;
	bool							m_actuality;
	bool							m_failed;
	bool							m_completed;
	bool							m_random;
	u32								m_curr_point_index;
	u32								m_prev_point_index;
	u32								m_start_point_index;
	Fvector3 							m_dest_position;
	CExtrapolateCallback			m_extrapolate_callback;
	CRestrictedObject				*m_object;
	CGameObject						*m_game_object;

protected:
	inline			bool				random					() const;
	inline			bool				accessible				(const Fvector3& position) const;
	inline			bool				accessible				(u32 vertex_id) const;
	inline			bool				accessible				(const CPatrolPath::CVertex *vertex) const;

public:
	inline								CPatrolPathManager		(CRestrictedObject *object, CGameObject *game_object);
		virtual						~CPatrolPathManager		();
		virtual	void				reinit					();
	inline			CExtrapolateCallback&extrapolate_callback	();
	inline			void				make_inactual			();
	inline			const CPatrolPath	*get_path				() const;
	inline			void				set_path				(const CPatrolPath *path, CSharedString path_name);
	inline			void				set_path				(CSharedString path_name);
	inline			void				set_path				(CSharedString path_name, const EPatrolStartType patrol_start_type = ePatrolStartTypeNearest, const EPatrolRouteType patrol_route_type = ePatrolRouteTypeContinue, bool random = true);
	inline			void				set_start_type			(const EPatrolStartType patrol_start_type);
	inline			void				set_route_type			(const EPatrolRouteType patrol_route_type);
	inline			void				set_random				(bool random);
	inline			bool				actual					() const;
	CSharedString			path_name				() const;
				void				set_previous_point		(int point_index);
				void				set_start_point			(int point_index);
	inline			bool				completed				() const;
	inline			bool				failed					() const;
				void				select_point			(const Fvector3& position, u32 &dest_vertex_id);
	inline			const Fvector3&	destination_position	() const;
	inline			u32					get_current_point_index	() const;
	inline			CRestrictedObject	&object					() const;
				bool				extrapolate_path		();

private:
				u32					get_next_point			(u32 prev_point_index);
				void				reset					();
};

#include "patrol_path_manager_inline.h"