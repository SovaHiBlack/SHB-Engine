//	Module 		: AlifeSwitchManager.h
//	Description : ALife Simulator switch manager

#pragma once

#include "alife_simulator_base.h"

class CAlifeSwitchManager : public virtual CAlifeSimulatorBase, CRandom
{
protected:
	typedef CAlifeSimulatorBase				inherited;
	typedef ALife::OBJECT_VECTOR			OBJECT_VECTOR;

	float			m_switch_distance;
	float			m_switch_factor;
	float			m_online_distance;
	float			m_offline_distance;

private:
	OBJECT_VECTOR	m_saved_chidren;

protected:
	bool	synchronize_location(CSE_ALifeDynamicObject* object);

public:
	void	try_switch_online(CSE_ALifeDynamicObject* object);
	void	try_switch_offline(CSE_ALifeDynamicObject* object);
	void	switch_online(CSE_ALifeDynamicObject* object);
	void	switch_offline(CSE_ALifeDynamicObject* object);
	void	remove_online(CSE_ALifeDynamicObject* object, bool update_registries = true);
	void	add_online(CSE_ALifeDynamicObject* object, bool update_registries = true);

	inline				CAlifeSwitchManager(CServer* server, const char* section);
	virtual			~CAlifeSwitchManager( );
	void	switch_object(CSE_ALifeDynamicObject* object);
	inline float	online_distance( ) const;
	inline float	offline_distance( ) const;
	inline float	switch_distance( ) const;
	inline void	set_switch_distance(float switch_distance);
	inline void	set_switch_factor(float switch_factor);
};

#include "AlifeSwitchManager_inline.h"
