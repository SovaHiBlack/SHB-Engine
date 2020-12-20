#pragma once

#include "CustomZone.h"
#include "..\ENGINE\SkeletonAnimated.h"
#include "ZoneVisual.h"

#include "script_export_space.h"

class CHairsZone : public CVisualZone
{
typedef				CVisualZone		inherited;

public:
	virtual			void		Affect				(SZoneObjectInfo* O)		;
	virtual			void		Load				(const char* section);

protected:
					float		m_min_speed_to_react;
	virtual			bool		BlowoutState		();
	virtual			void		CheckForAwaking		();

public:
	static void script_register(lua_State*);
};

add_to_type_list(CHairsZone)
#undef script_type_list
#define script_type_list save_type_list(CHairsZone)
