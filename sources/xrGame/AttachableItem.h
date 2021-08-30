//	Module 		: AttachableItem.h
//	Description : Attachable item

#pragma once

class CPHShellHolder;
class CInventoryItem;

class CAttachableItem
{
private:
	CInventoryItem*										m_item;
	CSharedString										m_bone_name;
	Fmatrix												m_offset;
	unsigned short										m_bone_id;
	bool												m_enabled;
//	bool												m_auto_attach;

#ifdef DEBUG
	bool												m_valid;
#endif // def DEBUG

public:
	inline						CAttachableItem			( );
	virtual						~CAttachableItem		( );
	virtual DLL_Pure*			_construct				( );
	virtual CAttachableItem*	cast_attachable_item	( )
	{
		return this;
	}
	virtual void				reload					(const char* section);
	virtual void				OnH_A_Chield			( );
	virtual void				OnH_A_Independent		( );
	virtual void				renderable_Render		( );
	virtual bool				can_be_attached			( ) const;
	virtual void				afterAttach				( );
	virtual void				afterDetach				( );
	inline CInventoryItem&		item					( ) const;
	inline CPHShellHolder&		object					( ) const;
	inline CSharedString		bone_name				( ) const;
	inline unsigned short		bone_id					( ) const;
	inline void					set_bone_id				(unsigned short bone_id);
	inline const Fmatrix&		offset					( ) const;
	inline bool					enabled					( ) const;
	void						enable					(bool value);

#ifdef DEBUG
	static CAttachableItem*								m_dbgItem;
	static Fvector3				get_angle_offset		( )
	{
		VERIFY(m_dbgItem);
		Fvector3 v;
		m_dbgItem->m_offset.getHPB(v);
		return v;
	}
	static Fvector3				get_pos_offset			( )
	{
		VERIFY(m_dbgItem);
		return m_dbgItem->m_offset.c;
	}
	static void					set_angle_offset		(Fvector3 val)
	{
		Fvector3 c = get_pos_offset( );
		m_dbgItem->m_offset.setHPB(VPUSH(val));
		m_dbgItem->m_offset.c = c;
	}

	static void					rot_dx					(float val)
	{
		Fvector3 v = get_angle_offset( );
		v.x += val;
		set_angle_offset(v);
	}
	static void					rot_dy					(float val)
	{
		Fvector3 v = get_angle_offset( );
		v.y += val;
		set_angle_offset(v);
	}
	static void					rot_dz					(float val)
	{
		Fvector3 v = get_angle_offset( );
		v.z += val;
		set_angle_offset(v);
	}

	static void					mov_dx					(float val)
	{
		Fvector3 c = get_pos_offset( );
		c.x += val;
		m_dbgItem->m_offset.c = c;
	}
	static void					mov_dy					(float val)
	{
		Fvector3 c = get_pos_offset( );
		c.y += val;
		m_dbgItem->m_offset.c = c;
	}
	static void					mov_dz					(float val)
	{
		Fvector3 c = get_pos_offset( );
		c.z += val;
		m_dbgItem->m_offset.c = c;
	}
#endif // def DEBUG

};

#include "AttachableItem_inline.h"
