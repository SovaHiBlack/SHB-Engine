////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_switch_manager.h
//	Created 	: 25.12.2002
//  Modified 	: 12.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife Simulator switch manager
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "alife_simulator_base.h"

class CALifeSwitchManager : public virtual CALifeSimulatorBase, CRandom {
protected:
	typedef CALifeSimulatorBase				inherited;
	typedef ALife::OBJECT_VECTOR			OBJECT_VECTOR;

protected:
	f32			m_switch_distance;
	f32			m_switch_factor;
	f32			m_online_distance;
	f32			m_offline_distance;

private:
	OBJECT_VECTOR	m_saved_chidren;

protected:
			bool	synchronize_location	(CSE_ALifeDynamicObject	*object);

public:
			void	try_switch_online		(CSE_ALifeDynamicObject	*object);
			void	try_switch_offline		(CSE_ALifeDynamicObject	*object);
			void	switch_online			(CSE_ALifeDynamicObject	*object);
			void	switch_offline			(CSE_ALifeDynamicObject	*object);
			void	remove_online			(CSE_ALifeDynamicObject	*object, bool update_registries = true);
			void	add_online				(CSE_ALifeDynamicObject	*object, bool update_registries = true);

public:
	IC				CALifeSwitchManager		(xrServer *server, pcstr section);
	virtual			~CALifeSwitchManager	();
			void	switch_object			(CSE_ALifeDynamicObject	*object);
	IC		f32	online_distance			() const;
	IC		f32	offline_distance		() const;
	IC		f32	switch_distance			() const;
	IC		void	set_switch_distance		(f32 switch_distance);
	IC		void	set_switch_factor		(f32 switch_factor);
};

#include "alife_switch_manager_inline.h"