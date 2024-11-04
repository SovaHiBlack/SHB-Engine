////////////////////////////////////////////////////////////////////////////
//	Module 		: level_changer.h
//	Created 	: 10.07.2003
//  Modified 	: 10.07.2003
//	Author		: Dmitriy Iassenev
//	Description : Level change object
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "GameObject.h"
#include "..\XR_3DA\feel_touch.h"
#include "game_graph_space.h"

class CLevelChanger : public CGameObject, public Feel::Touch {
private:
	typedef	CGameObject	inherited;

private:
	GameGraph::_GRAPH_ID	m_game_vertex_id;
	u32						m_level_vertex_id;
	fVector3					m_position;
	fVector3					m_angles;
	f32					m_entrance_time;
	void				update_actor_invitation	();
	bool					m_bSilentMode;
	bool				get_reject_pos(fVector3& p, fVector3& r);

public:
	virtual				~CLevelChanger		();
	virtual BOOL		net_Spawn			(CSE_Abstract* DC);
	virtual void		net_Destroy			();
	virtual void		Center				(fVector3& C) const;
	virtual f32			Radius				() const;
	virtual void		shedule_Update		(u32 dt);
	virtual void		feel_touch_new		(CObject* O);
	virtual BOOL		feel_touch_contact	(CObject* O);

	virtual bool		IsVisibleForZones	() {return false;}
};
