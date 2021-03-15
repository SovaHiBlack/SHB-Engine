////////////////////////////////////////////////////////////////////////////
//	Module 		: cover_point.h
//	Created 	: 24.03.2004
//  Modified 	: 29.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Cover point class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "script_export_space.h"

class CCoverPoint {
public:
	Fvector3				m_position;
	u32					m_level_vertex_id;

public:
	inline					CCoverPoint		(const Fvector3& point, u32 level_vertex_id);
	inline const Fvector3&	position		() const;
	inline	u32				level_vertex_id	() const;
	inline	bool			operator==		(const CCoverPoint &point) const;

public:
	static void script_register(lua_State*);
};

add_to_type_list(CCoverPoint)
#undef script_type_list
#define script_type_list save_type_list(CCoverPoint)

#include "cover_point_inline.h"
