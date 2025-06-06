////////////////////////////////////////////////////////////////////////////
//	Module 		: agent_location_manager.h
//	Created 	: 24.05.2004
//  Modified 	: 14.01.2005
//	Author		: Dmitriy Iassenev
//	Description : Agent location manager
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "danger_location.h"

class CStalker;
class CCoverPoint;
class CAgentManager;
class CDangerLocation;
class CObject;

class CAgentLocationManager {
public:
	typedef intrusive_ptr<CDangerLocation>				CDangerLocationPtr;
	typedef xr_vector<CDangerLocationPtr>				LOCATIONS;

private:
	CAgentManager				*m_object;
	LOCATIONS					m_danger_locations;

protected:
	IC		CAgentManager		&object					() const;
			void				remove_old_danger_covers();


public:
	IC							CAgentLocationManager	(CAgentManager *object);
	IC		CDangerLocationPtr	location				(const fVector3& position);
	IC		CDangerLocationPtr	location				(const CObject *object);
	IC		void				clear					();
			void				update					();
			void				add						(CDangerLocationPtr location);
			f32				danger(const CCoverPoint* cover, CStalker* member) const;
			bool				suitable(CStalker* object, const CCoverPoint* location, bool use_enemy_info) const;
			void				make_suitable(CStalker* object, const CCoverPoint* location) const;
			void				remove_links			(CObject *object);
	IC		const LOCATIONS		&locations				() const;
};

#include "agent_location_manager_inline.h"