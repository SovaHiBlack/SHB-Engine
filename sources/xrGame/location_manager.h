////////////////////////////////////////////////////////////////////////////
//	Module 		: location_manager.h
//	Created 	: 27.12.2003
//  Modified 	: 27.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Location manager
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "game_graph_space.h"

class CGameObject;

class CLocationManager {
private:
	GameGraph::TerrainPlaceVec	m_vertex_types;
	CGameObject					*m_object;

public:
	inline				CLocationManager			(CGameObject *object);
	virtual			~CLocationManager			();
	virtual void	Load						(const char* section);
	virtual void	reload						(const char* section);
	inline		const GameGraph::TerrainPlaceVec& vertex_types	() const;
};

#include "location_manager_inline.h"
