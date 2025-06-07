#pragma once
#include "ui\uistatic.h"

class CObject;
class CWeaponBinoculars;

enum
{
	flVisObjNotValid = (1 << 0),
	flTargetLocked = (1 << 1),
};

struct SBinocVisibleObj
{
	SBinocVisibleObj( )
	{ }
	CObject* m_object;
	CUIStatic				m_lt;
	CUIStatic				m_lb;
	CUIStatic				m_rt;
	CUIStatic				m_rb;
	fRect					cur_rect;

	f32					m_upd_speed;
	flags8					m_flags;
	void					create_default(u32 color);
	void					Draw( );
	void					Update( );
	bool					operator <				(const SBinocVisibleObj& other) const
	{
		return  m_flags.test(flVisObjNotValid) < other.m_flags.test(flVisObjNotValid);
	} //move non-actual to tail
};

class CBinocularsVision
{
	typedef xr_vector<SBinocVisibleObj*>	VIS_OBJECTS;
	typedef VIS_OBJECTS::iterator			VIS_OBJECTS_IT;
	VIS_OBJECTS								m_active_objects;

public:
	CBinocularsVision(CWeaponBinoculars* parent);
	~CBinocularsVision( );
	void	Update( );
	void	Draw( );
	void	remove_links(CObject* object);

protected:
	CWeaponBinoculars* m_parent;
	fColor						m_frame_color;
	f32						m_rotating_speed;
	void	Load(const shared_str& section);
	ref_sound					m_snd_found;
};
