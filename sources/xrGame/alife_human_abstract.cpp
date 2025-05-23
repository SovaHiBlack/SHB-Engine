////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_human_abstract.cpp
//	Created 	: 27.10.2005
//  Modified 	: 27.10.2005
//	Author		: Dmitriy Iassenev
//	Description : ALife human abstract class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "xrServer_objects_ALife_Monsters.h"
#include "alife_human_brain.h"
#include "alife_human_object_handler.h"
#include "AISpace.h"
#include "alife_simulator.h"
#include "alife_group_registry.h"

void CSE_ALifeHumanAbstract::update									()
{
	if (!bfActive())
		return;

	brain().update							();
}

bool CSE_ALifeHumanAbstract::bfPerformAttack						()
{
	return									(brain().perform_attack());
}

ALife::EMeetActionType CSE_ALifeHumanAbstract::tfGetActionType		(CSE_ALifeSchedulable *schedulable, s32 iGroupIndex, bool bMutualDetection)
{
	return									(brain().action_type(schedulable,iGroupIndex,bMutualDetection));
}

void CSE_ALifeHumanAbstract::vfDetachAll							(bool bFictitious)
{
	brain().objects().detach_all			(bFictitious);
}

void CSE_ALifeHumanAbstract::vfUpdateWeaponAmmo						()
{
	brain().objects().update_weapon_ammo	();
}

void CSE_ALifeHumanAbstract::vfProcessItems							()
{
	brain().objects().process_items			();
}

void CSE_ALifeHumanAbstract::vfAttachItems							(ALife::ETakeType tTakeType)
{
	brain().objects().attach_items			();
}

CSE_ALifeDynamicObject *CSE_ALifeHumanAbstract::tpfGetBestDetector	()
{
	return									(brain().objects().best_detector());
}

CSE_ALifeItemWeapon *CSE_ALifeHumanAbstract::tpfGetBestWeapon		(ALife::EHitType &tHitType, f32& fHitPower)
{
	return									(brain().objects().best_weapon());
}

void CSE_ALifeHumanAbstract::on_register							()
{
	inherited2::on_register					();
	brain().on_register						();
	// because we need to load profile to setup graph vertex masks
	specific_character						();
}

void CSE_ALifeHumanAbstract::on_unregister							()
{
	brain().on_unregister					();
	if (m_group_id != 0xffff)
		ai().alife().groups().object(m_group_id).unregister_member	(ID);
}

void CSE_ALifeHumanAbstract::spawn_supplies							()
{
	specific_character			();
	inherited1::spawn_supplies	();
	inherited2::spawn_supplies	();
}

void CSE_ALifeHumanAbstract::add_online								(const bool &update_registries)
{
	CSE_ALifeTraderAbstract::add_online		(update_registries);
	brain().on_switch_online				();
}

void CSE_ALifeHumanAbstract::add_offline							(const xr_vector<ALife::_OBJECT_ID> &saved_children, const bool &update_registries)
{
	CSE_ALifeTraderAbstract::add_offline	(saved_children,update_registries);
	brain().on_switch_offline				();
}
