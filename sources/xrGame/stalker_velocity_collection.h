////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_velocity_collection.h
//	Created 	: 13.12.2005
//  Modified 	: 13.12.2005
//	Author		: Dmitriy Iassenev
//	Description : Stalker velocity collection
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ai_monster_space.h"

class CStalkerVelocityCollection {
private:
	f32				m_danger[2][2][4];
	f32				m_free[2];
	f32				m_panic;

public:
						CStalkerVelocityCollection	(const shared_str &section);
	IC	f32		velocity					(
		const MonsterSpace::EMentalState		&mental_state,
		const MonsterSpace::EBodyState			&body_state,
		const MonsterSpace::EMovementType		&movement_type,
		const MonsterSpace::EMovementDirection	&movement_direction
	) const;
};

#include "stalker_velocity_collection_inline.h"
