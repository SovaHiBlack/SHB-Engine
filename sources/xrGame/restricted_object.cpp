////////////////////////////////////////////////////////////////////////////
//	Module 		: restricted_object.cpp
//	Created 	: 18.08.2004
//  Modified 	: 23.08.2004
//	Author		: Dmitriy Iassenev
//	Description : Restricted object
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "restricted_object.h"
#include "space_restriction_manager.h"
#include "xrServer_Objects_ALife_Monsters.h"
#include "level.h"
#include "AISpace.h"
#include "level_graph.h"
#include "space_restriction.h"
#include "space_restriction_bridge.h"
#include "space_restriction_base.h"
#include "Profiler.h"
#include "alife_simulator.h"
#include "alife_object_registry.h"
#include "game_graph.h"
#include "custommonster.h"

CRestrictedObject::~CRestrictedObject		()
{
}

IC	void construct_string					(pstr result, const xr_vector<ALife::_OBJECT_ID> &restrictions)
{
	u32		count = xr_strlen(result) ? _GetItemCount(result) : 0;
	xr_vector<ALife::_OBJECT_ID>::const_iterator	I = restrictions.begin();
	xr_vector<ALife::_OBJECT_ID>::const_iterator	E = restrictions.end();
	for ( ; I != E; ++I) {
		CSE_ALifeDynamicObject	*object = ai().alife().objects().object(*I);
		if (ai().game_graph().vertex(object->m_tGraphID)->level_id() != ai().level_graph().level_id())
			continue;

		if (count)
			strcat(result,",");
		strcat(result,object->name_replace());
		++count;
	}
}

#if 0
IC	void construct_id_string					(pstr result, const xr_vector<ALife::_OBJECT_ID> &restrictions)
{
	strcpy			(result,"");
	string16		temp;
	u32				count = 0;
	xr_vector<ALife::_OBJECT_ID>::const_iterator	I = restrictions.begin();
	xr_vector<ALife::_OBJECT_ID>::const_iterator	E = restrictions.end();
	for ( ; I != E; ++I) {
		if (count)
			strcat	(result,",");
		sprintf_s		(temp,"%d",*I);
		strcat		(result,temp);
		++count;
	}
}
#endif

BOOL CRestrictedObject::net_Spawn			(CSE_Abstract* data)
{
	CSE_Abstract				*abstract	= (CSE_Abstract*)(data);
	CSE_ALifeMonsterAbstract	*monster	= smart_cast<CSE_ALifeMonsterAbstract*>(abstract);
	VERIFY						(monster);
	m_applied					= false;
	m_removed					= true;
	
	string4096					temp0;
	string4096					temp1;
	
	strcpy						(temp0,*monster->m_out_space_restrictors);
	strcpy						(temp1,*monster->m_in_space_restrictors);

	if (ai().get_alife()) {
		construct_string		(temp0,monster->m_dynamic_out_restrictions);
		construct_string		(temp1,monster->m_dynamic_in_restrictions);
	}

#if 0
	string4096					temp2;
	string4096					temp3;

	construct_id_string			(temp2,monster->m_dynamic_out_restrictions);
	construct_id_string			(temp3,monster->m_dynamic_in_restrictions);

	Msg							("Restricting object %s with",monster->name_replace());
	Msg							("STATIC OUT  : %s",*monster->m_out_space_restrictors);
	Msg							("STATIC IN   : %s",*monster->m_in_space_restrictors);
	Msg							("DYNAMIC OUT : %s",temp2);
	Msg							("DYNAMIC IN  : %s",temp3);
	Msg							("OUT         : %s",temp0);
	Msg							("IN          : %s",temp1);
#endif

	Level().space_restriction_manager().restrict	(monster->ID,temp0,temp1);

	actual						(true);
	
	return						(TRUE);
}

void CRestrictedObject::net_Destroy			()
{
	Level().space_restriction_manager().unrestrict	(m_object->ID());
}

u32	CRestrictedObject::accessible_nearest	(const fVector3& position, fVector3& result) const
{
	START_PROFILE("Restricted Object/Accessible Nearest");
	VERIFY						(!accessible(position));
	return						(Level().space_restriction_manager().accessible_nearest(object().ID(),position,result));
	STOP_PROFILE;
}

bool CRestrictedObject::accessible			(const fVector3& position) const
{
	START_PROFILE("Restricted Object/Accessible");
	return						(accessible(position, EPSILON_3));
	STOP_PROFILE;
}

bool CRestrictedObject::accessible			(const fVector3& position, f32 radius) const
{
	START_PROFILE("Restricted Object/Accessible");
	fSphere						sphere;
	sphere.P					= position;
	sphere.R					= radius;
	return						(Level().space_restriction_manager().accessible(object().ID(),sphere));
	STOP_PROFILE;
}

bool CRestrictedObject::accessible			(u32 level_vertex_id) const
{
	START_PROFILE("Restricted Object/Accessible");
	VERIFY						(ai().level_graph().valid_vertex_id(level_vertex_id));
	return						(accessible(level_vertex_id, EPSILON_3));
	STOP_PROFILE;
}

bool CRestrictedObject::accessible			(u32 level_vertex_id, f32 radius) const
{
	START_PROFILE("Restricted Object/Accessible");
	VERIFY						(ai().level_graph().valid_vertex_id(level_vertex_id));
	return						(Level().space_restriction_manager().accessible(object().ID(),level_vertex_id,radius));
	STOP_PROFILE;
}

void CRestrictedObject::add_border			(u32 start_vertex_id, f32 radius) const
{
	START_PROFILE("Restricted Object/Add Border");	
	
	VERIFY						(ai().level_graph().valid_vertex_id(start_vertex_id));
	VERIFY						(!m_applied);
	VERIFY						(m_removed);
	m_removed					= false;
	if (accessible(start_vertex_id)) {
		m_applied				= true;
		Level().space_restriction_manager().add_border(object().ID(),start_vertex_id,radius);
	}
	
	STOP_PROFILE;
}

void CRestrictedObject::add_border			(const fVector3& start_position, const fVector3& dest_position) const
{
	START_PROFILE("Restricted Object/Add Border");	

	VERIFY						(!m_applied);
	VERIFY						(m_removed);
	m_removed					= false;
	if (accessible(start_position)) {
		m_applied				= true;
		Level().space_restriction_manager().add_border(object().ID(),start_position,dest_position);
	}

	STOP_PROFILE;
}

void CRestrictedObject::add_border			(u32 start_vertex_id, u32 dest_vertex_id) const
{
	START_PROFILE("Restricted Object/Add Border");	
	VERIFY						(ai().level_graph().valid_vertex_id(start_vertex_id));
	VERIFY						(!m_applied);
	VERIFY						(m_removed);
	m_removed					= false;
	if (accessible(start_vertex_id)) {
		m_applied				= true;
		Level().space_restriction_manager().add_border(object().ID(),start_vertex_id,dest_vertex_id);
	}
	STOP_PROFILE;
}

void CRestrictedObject::remove_border		() const
{
	START_PROFILE("Restricted Object/Remove Border");
	
	VERIFY						(!m_removed);
	m_removed					= true;
	if (m_applied)
		Level().space_restriction_manager().remove_border(object().ID());
	m_applied					= false;
	
	STOP_PROFILE;
}

shared_str	CRestrictedObject::in_restrictions	() const
{
	START_PROFILE("Restricted Object/in_restrictions")
	return						(Level().space_restriction_manager().in_restrictions(object().ID()));
	STOP_PROFILE
}

shared_str CRestrictedObject::out_restrictions	() const
{
	START_PROFILE("Restricted Object/out_restrictions")
	return						(Level().space_restriction_manager().out_restrictions(object().ID()));
	STOP_PROFILE
}

shared_str CRestrictedObject::base_in_restrictions	() const
{
	START_PROFILE("Restricted Object/base_in_restrictions")
	return						(Level().space_restriction_manager().base_in_restrictions(object().ID()));
	STOP_PROFILE
}

shared_str CRestrictedObject::base_out_restrictions	() const
{
	START_PROFILE("Restricted Object/out_restrictions")
	return						(Level().space_restriction_manager().base_out_restrictions(object().ID()));
	STOP_PROFILE
}

IC	void CRestrictedObject::add_object_restriction(ALife::_OBJECT_ID id, const RestrictionSpace::ERestrictorTypes &restrictor_type)
{
	CNetPacket			net_packet;
	object().u_EventGen	(net_packet,GE_ADD_RESTRICTION,object().ID());
	net_packet.w		(&id,sizeof(id));
	net_packet.w		(&restrictor_type,sizeof(restrictor_type));
	Level().Send		(net_packet,net_flags(TRUE,TRUE));
}

IC	void CRestrictedObject::remove_object_restriction(ALife::_OBJECT_ID id, const RestrictionSpace::ERestrictorTypes &restrictor_type)
{
	CNetPacket			net_packet;
	object().u_EventGen	(net_packet,GE_REMOVE_RESTRICTION,object().ID());
	net_packet.w		(&id,sizeof(id));
	net_packet.w		(&restrictor_type,sizeof(restrictor_type));
	Level().Send		(net_packet,net_flags(TRUE,TRUE));
}

template <typename P, bool value>
IC	void CRestrictedObject::construct_restriction_string(pstr temp_restrictions, const xr_vector<ALife::_OBJECT_ID> &restrictions, shared_str current_restrictions, const P &p)
{
	u32							count = 0;
	strcpy						(temp_restrictions,"");
	xr_vector<ALife::_OBJECT_ID>::const_iterator	I = restrictions.begin();
	xr_vector<ALife::_OBJECT_ID>::const_iterator	E = restrictions.end();
	for ( ; I != E; ++I) {
		CObject					*object = Level().Objects.net_Find(*I);
		if (!object || !!strstr(*current_restrictions,*object->cName()) == value)
			continue;

		p						(this,object->ID());

		if (count)
			strcat(temp_restrictions,",");

		strcat(temp_restrictions,*object->cName());

		count++;
	}
}

template <bool add>
struct CRestrictionPredicate {
	RestrictionSpace::ERestrictorTypes m_restrictor_type;

    IC	CRestrictionPredicate	(RestrictionSpace::ERestrictorTypes restrictor_type)
	{
		m_restrictor_type = restrictor_type;
	}

	IC	void operator()			(CRestrictedObject *object, ALife::_OBJECT_ID id) const
	{
		if (add)
			object->add_object_restriction(id,m_restrictor_type);
		else
			object->remove_object_restriction(id,m_restrictor_type);
	}
};

void CRestrictedObject::add_restrictions	(const xr_vector<ALife::_OBJECT_ID> &out_restrictions, const xr_vector<ALife::_OBJECT_ID> &in_restrictions)
{
	if (out_restrictions.empty() && in_restrictions.empty())
		return;

	START_PROFILE("Restricted Object/Add Restrictions");

	string4096					temp_out_restrictions;
	string4096					temp_in_restrictions;
	
	construct_restriction_string<CRestrictionPredicate<true>,true>(temp_out_restrictions,out_restrictions,this->out_restrictions(),CRestrictionPredicate<true>(RestrictionSpace::eRestrictorTypeOut));
	construct_restriction_string<CRestrictionPredicate<true>,true>(temp_in_restrictions,in_restrictions,this->in_restrictions(),CRestrictionPredicate<true>(RestrictionSpace::eRestrictorTypeIn));

	Level().space_restriction_manager().add_restrictions	(object().ID(),temp_out_restrictions,temp_in_restrictions);

	actual						(false);
	
	STOP_PROFILE;
}

void CRestrictedObject::remove_restrictions	(const xr_vector<ALife::_OBJECT_ID> &out_restrictions, const xr_vector<ALife::_OBJECT_ID> &in_restrictions)
{
	if (out_restrictions.empty() && in_restrictions.empty())
		return;

	START_PROFILE("Restricted Object/Remove Restrictions");
	
	string4096					temp_out_restrictions;
	string4096					temp_in_restrictions;

	construct_restriction_string<CRestrictionPredicate<false>,false>(temp_out_restrictions,out_restrictions,this->out_restrictions(),CRestrictionPredicate<false>(RestrictionSpace::eRestrictorTypeOut));
	construct_restriction_string<CRestrictionPredicate<false>,false>(temp_in_restrictions,in_restrictions,this->in_restrictions(),CRestrictionPredicate<false>(RestrictionSpace::eRestrictorTypeIn));

	Level().space_restriction_manager().remove_restrictions	(object().ID(),temp_out_restrictions,temp_in_restrictions);
	
	actual						(false);

	STOP_PROFILE;
}

void CRestrictedObject::add_restrictions	(const shared_str &out_restrictions, const shared_str &in_restrictions)
{
	if (!out_restrictions.size() && !in_restrictions.size())
		return;

	START_PROFILE("Restricted Object/Add Restrictions");
	
	Level().space_restriction_manager().add_restrictions	(object().ID(),*out_restrictions,*in_restrictions);

	actual						(false);
	
	STOP_PROFILE;
}

void CRestrictedObject::remove_restrictions	(const shared_str &out_restrictions, const shared_str &in_restrictions)
{
	if (!out_restrictions.size() && !in_restrictions.size())
		return;

	START_PROFILE("Restricted Object/Remove Restrictions");
	
	Level().space_restriction_manager().remove_restrictions	(object().ID(),*out_restrictions,*in_restrictions);
	
	actual						(false);

	STOP_PROFILE;
}

void CRestrictedObject::remove_all_restrictions	(const RestrictionSpace::ERestrictorTypes &restrictor_type)
{
	CNetPacket			net_packet;
	object().u_EventGen	(net_packet,GE_REMOVE_ALL_RESTRICTIONS,object().ID());
	net_packet.w		(&restrictor_type,sizeof(restrictor_type));
	Level().Send		(net_packet,net_flags(TRUE,TRUE));
}

void CRestrictedObject::remove_all_restrictions	()
{
	START_PROFILE("Restricted Object/Remove Restrictions");
	
	remove_all_restrictions	(RestrictionSpace::eRestrictorTypeOut);
	remove_all_restrictions	(RestrictionSpace::eRestrictorTypeIn);

	if (Level().space_restriction_manager().in_restrictions(object().ID()).size() || Level().space_restriction_manager().out_restrictions(object().ID()).size())
		actual			(false);

	Level().space_restriction_manager().restrict(object().ID(),"","");
	
	STOP_PROFILE;
}

void CRestrictedObject::actual					(bool value)
{
	m_actual			= value;
	if (!actual())
		m_object->on_restrictions_change	();
}

