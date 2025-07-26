////////////////////////////////////////////////////////////////////////////
//	Module 		: ActorMemory.h
//	Description : actor memory
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "VisionClient.h"

class CActor;

class CActorMemory : public CVisionClient
{
private:
	typedef CVisionClient					inherited;

private:
	CActor*									m_actor;

public:
					CActorMemory			(CActor* actor);
	virtual BOOL	feel_vision_isRelevant	(CObject* object);
	virtual void	camera					(fVector3& position,
											   fVector3& direction,
											   fVector3& normal,
											   f32& field_of_view,
											   f32& aspect_ratio,
											   f32& near_plane,
											   f32& far_plane);
};
