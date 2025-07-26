////////////////////////////////////////////////////////////////////////////
//	Module 		: VisionClient_inline.h
//	Description : vision client inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once
#ifndef VISION_CLIENT_INLINE_H
#define VISION_CLIENT_INLINE_H

IC CVisualMemoryManager& CVisionClient::visual( ) const
{
	VERIFY(m_visual);
	return *m_visual;
}

#endif // VISION_CLIENT_INLINE_H