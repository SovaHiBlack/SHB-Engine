////////////////////////////////////////////////////////////////////////////
//	Module 		: AttachableItem.h
//	Created 	: 11.02.2004
//  Modified 	: 11.02.2004
//	Author		: Dmitriy Iassenev
//	Description : Attachable item
////////////////////////////////////////////////////////////////////////////

#pragma once

class CPhysicsShellHolder;
class CInventoryItem;

class CAttachableItem
{
private:
	CInventoryItem* m_item;
	shared_str					m_bone_name;
	fMatrix4x4						m_offset;
	u16							m_bone_id;
	bool						m_enabled;

#ifdef DEBUG
	bool						m_valid;
#endif

public:
	IC							CAttachableItem( );
	virtual						~CAttachableItem( );
	virtual	DLL_Pure* _construct( );
	virtual CAttachableItem* cast_attachable_item( )
	{
		return this;
	}
	virtual	void				reload(pcstr section);
	virtual void				OnH_A_Chield( );
	virtual void				OnH_A_Independent( );
	virtual void				renderable_Render( );
	virtual bool				can_be_attached( ) const;
	virtual	void				afterAttach( );
	virtual	void				afterDetach( );
	IC		CInventoryItem& item( ) const;
	IC		CPhysicsShellHolder& object( ) const;
	IC		shared_str			bone_name( ) const;
	IC		u16					bone_id( ) const;
	IC		void				set_bone_id(u16 bone_id);
	IC		const fMatrix4x4& offset( ) const;
	IC		bool				enabled( ) const;
	void				enable(bool value);

public:
#ifdef DEBUG
	static CAttachableItem* m_dbgItem;
	static fVector3				get_angle_offset( )
	{
		VERIFY(m_dbgItem);
		fVector3 v;
		m_dbgItem->m_offset.getHPB(v);
		return v;
	}
	static fVector3				get_pos_offset( )
	{
		VERIFY(m_dbgItem);
		return m_dbgItem->m_offset.c;
	}
	static	void				set_angle_offset(fVector3 val)
	{
		fVector3 c = get_pos_offset( );
		m_dbgItem->m_offset.setHPB(VPUSH(val));
		m_dbgItem->m_offset.c = c;
	}

	static	void				rot_dx(f32 val)
	{
		fVector3 v = get_angle_offset( );
		v.x += val;
		set_angle_offset(v);
	}
	static	void				rot_dy(f32 val)
	{
		fVector3 v = get_angle_offset( );
		v.y += val;
		set_angle_offset(v);
	}
	static	void				rot_dz(f32 val)
	{
		fVector3 v = get_angle_offset( );
		v.z += val;
		set_angle_offset(v);
	}

	static	void				mov_dx(f32 val)
	{
		fVector3 c = get_pos_offset( );
		c.x += val;
		m_dbgItem->m_offset.c = c;
	}
	static	void				mov_dy(f32 val)
	{
		fVector3 c = get_pos_offset( );
		c.y += val;
		m_dbgItem->m_offset.c = c;
	}
	static	void				mov_dz(f32 val)
	{
		fVector3 c = get_pos_offset( );
		c.z += val;
		m_dbgItem->m_offset.c = c;
	}
#endif
};

#include "AttachableItem_inline.h"
