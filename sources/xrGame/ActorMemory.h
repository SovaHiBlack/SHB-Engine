//	Module 		: ActorMemory.h
//	Description : actor memory

#pragma once

#include "vision_client.h"

class CActor;

class CActorMemory : public vision_client
{
private:
	using inherited							= vision_client;

	CActor*									m_actor;

public:
					CActorMemory			(CActor* actor);
	virtual BOOL	feel_vision_isRelevant	(CObject* object);
	virtual void	camera					(Fvector3& position, Fvector3& direction, Fvector3& normal, float& field_of_view, float& aspect_ratio, float& near_plane, float& far_plane);
};
