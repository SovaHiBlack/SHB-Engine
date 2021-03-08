//	Module 		: AttachableItem_inline.h
//	Description : Attachable item inline functions

#pragma once

inline CAttachableItem::CAttachableItem( )
{
	m_item				= nullptr;
	m_offset.identity	( );
	m_bone_name			= "";
	m_enabled			= true;
//	m_auto_attach		= true;

#ifdef DEBUG
	m_valid				= false;
#endif

}

inline	shared_str CAttachableItem::bone_name			() const
{
	VERIFY				(m_valid);
	return m_bone_name;
}

inline	const Fmatrix &CAttachableItem::offset			() const
{
	VERIFY				(m_valid);
	return m_offset;
}

inline	u16	 CAttachableItem::bone_id					() const
{
	VERIFY				(m_valid);
	return m_bone_id;
}

inline	void CAttachableItem::set_bone_id				(u16 bone_id)
{
	VERIFY				(m_valid);
	m_bone_id			= bone_id;
}

inline	bool CAttachableItem::enabled					() const
{
//	VERIFY				(m_valid);
	return m_enabled;
}

inline	CInventoryItem &CAttachableItem::item			() const
{
	VERIFY				(m_item);
	return *m_item;
}
