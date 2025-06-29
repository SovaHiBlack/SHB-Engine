#include "stdafx.h"
#include "game_sv_single.h"
#include "xrserver_objects_alife_monsters.h"
#include "alife_simulator.h"
#include "alife_object_registry.h"
#include "alife_graph_registry.h"
#include "alife_time_manager.h"
#include "../xrNetServer/net_utils.h"
#include "object_broker.h"
#include "gamepersistent.h"
#include "xrServer.h"
#include "..\XR_3DA\x_ray.h"

game_sv_Single::game_sv_Single			()
{
	m_alife_simulator			= NULL;
	m_type						= GAME_SINGLE;
};

game_sv_Single::~game_sv_Single			()
{
	delete_data					(m_alife_simulator);
}

void	game_sv_Single::Create			(shared_str& options)
{
	inherited::Create					(options);

	if (strstr(*options,"/alife"))
		m_alife_simulator				= xr_new<CALifeSimulator>(&server(),&options);

	switch_Phase						(GAME_PHASE_INPROGRESS);
}

void	game_sv_Single::OnCreate		(u16 id_who)
{
	if (!ai().get_alife())
		return;

	CSE_Abstract			*e_who			= get_entity_from_eid(id_who);
	VERIFY					(e_who);
	if (!e_who->m_bALifeControl)
		return;

	CSE_ALifeObject			*alife_object	= smart_cast<CSE_ALifeObject*>(e_who);
	if (!alife_object)
		return;

	alife_object->m_bOnline	= true;

	if (alife_object->ID_Parent != 0xffff) {
		CSE_ALifeDynamicObject			*parent = ai().alife().objects().object(alife_object->ID_Parent,true);
		if (parent) {
			CSE_ALifeTraderAbstract		*trader = smart_cast<CSE_ALifeTraderAbstract*>(parent);
			if (trader)
				alife().create			(alife_object);
			else
				alife_object->m_bALifeControl	= false;
		}
		else
			alife_object->m_bALifeControl		= false;
	}
	else
		alife().create					(alife_object);
}

BOOL	game_sv_Single::OnTouch			(u16 eid_who, u16 eid_what, BOOL bForced)
{
	CSE_Abstract*		e_who	= get_entity_from_eid(eid_who);		VERIFY(e_who	);
	CSE_Abstract*		e_what	= get_entity_from_eid(eid_what);	VERIFY(e_what	);

	if (ai().get_alife()) {
		CSE_ALifeInventoryItem	*l_tpALifeInventoryItem	= smart_cast<CSE_ALifeInventoryItem*>	(e_what);
		CSE_ALifeDynamicObject	*l_tpDynamicObject		= smart_cast<CSE_ALifeDynamicObject*>	(e_who);
		
		if	(
				l_tpALifeInventoryItem && 
				l_tpDynamicObject && 
				ai().alife().graph().level().object(l_tpALifeInventoryItem->base()->ID,true) &&
				ai().alife().objects().object(e_who->ID,true) &&
				ai().alife().objects().object(e_what->ID,true)
			)
			alife().graph().attach	(*e_who,l_tpALifeInventoryItem,l_tpDynamicObject->m_tGraphID,false,false);
#ifdef DEBUG
		else
			if (psAI_Flags.test(aiALife)) {
				Msg				("Cannot attach object [%s][%s][%d] to object [%s][%s][%d]",e_what->name_replace(),*e_what->s_name,e_what->ID,e_who->name_replace(),*e_who->s_name,e_who->ID);
			}
#endif
	}
	return TRUE;
}

void game_sv_Single::OnDetach(u16 eid_who, u16 eid_what)
{
	if (ai().get_alife()) 
	{
		CSE_Abstract*		e_who	= get_entity_from_eid(eid_who);		VERIFY(e_who	);
		CSE_Abstract*		e_what	= get_entity_from_eid(eid_what);	VERIFY(e_what	);

		CSE_ALifeInventoryItem *l_tpALifeInventoryItem = smart_cast<CSE_ALifeInventoryItem*>(e_what);
		if (!l_tpALifeInventoryItem)
			return;

		CSE_ALifeDynamicObject *l_tpDynamicObject = smart_cast<CSE_ALifeDynamicObject*>(e_who);
		if (!l_tpDynamicObject)
			return;
		
		if	(
				ai().alife().objects().object(e_who->ID,true) && 
				!ai().alife().graph().level().object(l_tpALifeInventoryItem->base()->ID,true) && 
				ai().alife().objects().object(e_what->ID,true)
			)
			alife().graph().detach(*e_who,l_tpALifeInventoryItem,l_tpDynamicObject->m_tGraphID,false,false);
		else {
			if (!ai().alife().objects().object(e_what->ID,true)) {
				u16				id = l_tpALifeInventoryItem->base()->ID_Parent;
				l_tpALifeInventoryItem->base()->ID_Parent	= 0xffff;
				
				CSE_ALifeDynamicObject *dynamic_object = smart_cast<CSE_ALifeDynamicObject*>(e_what);
				VERIFY			(dynamic_object);
				dynamic_object->m_tNodeID		= l_tpDynamicObject->m_tNodeID;
				dynamic_object->m_tGraphID		= l_tpDynamicObject->m_tGraphID;
				dynamic_object->m_bALifeControl	= true;
				dynamic_object->m_bOnline		= true;
				alife().create	(dynamic_object);
				l_tpALifeInventoryItem->base()->ID_Parent	= id;
			}
#ifdef DEBUG
			else
				if (psAI_Flags.test(aiALife)) {
					Msg			("Cannot detach object [%s][%s][%d] from object [%s][%s][%d]",l_tpALifeInventoryItem->base()->name_replace(),*l_tpALifeInventoryItem->base()->s_name,l_tpALifeInventoryItem->base()->ID,l_tpDynamicObject->base()->name_replace(),l_tpDynamicObject->base()->s_name,l_tpDynamicObject->ID);
				}
#endif
		}
	}
}


void	game_sv_Single::Update			()
{
	inherited::Update	();
}

ALife::_TIME_ID game_sv_Single::GetGameTime		()
{
	if (ai().get_alife() && ai().alife().initialized())
		return(ai().alife().time_manager().game_time());
	else
		return(inherited::GetGameTime());
}

f32 game_sv_Single::GetGameTimeFactor		()
{
	if (ai().get_alife() && ai().alife().initialized())
		return(ai().alife().time_manager().time_factor());
	else
		return(inherited::GetGameTimeFactor());
}

void game_sv_Single::SetGameTimeFactor		(const f32 fTimeFactor)
{
	if (ai().get_alife() && ai().alife().initialized())
		return(alife().time_manager().set_time_factor(fTimeFactor));
	else
		return(inherited::SetGameTimeFactor(fTimeFactor));
}

ALife::_TIME_ID game_sv_Single::GetEnvironmentGameTime		()
{
	return(inherited::GetGameTime());
}

f32 game_sv_Single::GetEnvironmentGameTimeFactor		()
{
	return(inherited::GetGameTimeFactor());
}

void game_sv_Single::SetEnvironmentGameTimeFactor		(const f32 fTimeFactor)
{
//	return(inherited::SetGameTimeFactor(fTimeFactor));
}

bool game_sv_Single::change_level					(CNetPacket& net_packet, CClientID sender)
{
	if (ai().get_alife())
		return					(alife().change_level(net_packet));
	else
		return					(true);
}

void game_sv_Single::save_game						(CNetPacket& net_packet, CClientID sender)
{
	if (!ai().get_alife())
		return;
	
	alife().save				(net_packet);
}

bool game_sv_Single::load_game					(CNetPacket& net_packet, CClientID sender)
{
	if (!ai().get_alife())
		return					(inherited::load_game(net_packet,sender));
	shared_str						game_name;
	net_packet.r_stringZ		(game_name);
	return						(alife().load_game(*game_name,true));
}

void game_sv_Single::reload_game				(CNetPacket& net_packet, CClientID sender)
{
}

void game_sv_Single::switch_distance			(CNetPacket& net_packet, CClientID sender)
{
	if (!ai().get_alife())
		return;

	alife().set_switch_distance	(net_packet.r_float());
}

void game_sv_Single::teleport_object			(CNetPacket& net_packet, u16 id)
{
	if (!ai().get_alife())
		return;
	
	GameGraph::_GRAPH_ID		game_vertex_id;
	u32						level_vertex_id;
	fVector3					position;

	net_packet.r			(&game_vertex_id,sizeof(game_vertex_id));
	net_packet.r			(&level_vertex_id,sizeof(level_vertex_id));
	net_packet.r_vec3		(position);

	alife().teleport_object (id,game_vertex_id,level_vertex_id,position);
}

void game_sv_Single::add_restriction			(CNetPacket& packet, u16 id)
{
	if (!ai().get_alife())
		return;

	ALife::_OBJECT_ID		restriction_id;
	packet.r				(&restriction_id,sizeof(restriction_id));

	RestrictionSpace::ERestrictorTypes	restriction_type;
	packet.r				(&restriction_type,sizeof(restriction_type));
	
	alife().add_restriction (id,restriction_id,restriction_type);
}

void game_sv_Single::remove_restriction			(CNetPacket& packet, u16 id)
{
	if (!ai().get_alife())
		return;

	ALife::_OBJECT_ID		restriction_id;
	packet.r				(&restriction_id,sizeof(restriction_id));

	RestrictionSpace::ERestrictorTypes	restriction_type;
	packet.r				(&restriction_type,sizeof(restriction_type));
	
	alife().remove_restriction (id,restriction_id,restriction_type);
}

void game_sv_Single::remove_all_restrictions	(CNetPacket& packet, u16 id)
{
	if (!ai().get_alife())
		return;

	RestrictionSpace::ERestrictorTypes	restriction_type;
	packet.r				(&restriction_type,sizeof(restriction_type));

	alife().remove_all_restrictions (id,restriction_type);
}

void game_sv_Single::sls_default				()
{
	alife().update_switch	();
}

shared_str game_sv_Single::level_name			(const shared_str &server_options) const
{
	if (!ai().get_alife())
		return				(inherited::level_name(server_options));
	return					(alife().level_name());
}

void game_sv_Single::on_death					(CSE_Abstract *e_dest, CSE_Abstract *e_src)
{
	inherited::on_death		(e_dest,e_src);

	if (!ai().get_alife())
		return;

	alife().on_death		(e_dest,e_src);
}

void game_sv_Single::restart_simulator			(pcstr saved_game_name)
{
	shared_str				&options = *alife().server_command_line();

	delete_data				(m_alife_simulator);
	server().clear_ids		();

	strcpy					(g_pGamePersistent->m_game_params.m_game_or_spawn,saved_game_name);
	strcpy					(g_pGamePersistent->m_game_params.m_new_or_load,"load");

	pApp->LoadBegin			();
	m_alife_simulator		= xr_new<CALifeSimulator>(&server(),&options);
	g_pGamePersistent->LoadTitle		("st_client_synchronising");
	Device.PreCache			(30);
	pApp->LoadEnd			();
}