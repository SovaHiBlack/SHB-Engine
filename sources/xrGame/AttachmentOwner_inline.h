//	Module 		: AttachmentOwner_inline.h
//	Description : Attachment owner inline functions

#pragma once

inline	CAttachmentOwner::CAttachmentOwner( )
{ }

inline	const xr_vector<CAttachableItem*>& CAttachmentOwner::attached_objects( ) const
{
	return m_attached_objects;
}
