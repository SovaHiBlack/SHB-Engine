////////////////////////////////////////////////////////////////////////////
//	Module 		: EatableItemObject.cpp
//	Description : Eatable item object implementation
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EatableItemObject.h"

CEatableItemObject::CEatableItemObject( )
{ }

CEatableItemObject::~CEatableItemObject( )
{ }

DLL_Pure* CEatableItemObject::_construct( )
{
	CEatableItem::_construct( );
	CPhysicItem::_construct( );
	return this;
}

void CEatableItemObject::Load(pcstr section)
{
	CPhysicItem::Load(section);
	CEatableItem::Load(section);
}

void CEatableItemObject::Hit(SHit* pHDS)
{
	CPhysicItem::Hit(pHDS);
	CEatableItem::Hit(pHDS);
}

void CEatableItemObject::OnH_A_Independent( )
{
	CEatableItem::OnH_A_Independent( );
	CPhysicItem::OnH_A_Independent( );
}

void CEatableItemObject::OnH_B_Independent(bool just_before_destroy)
{
	CEatableItem::OnH_B_Independent(just_before_destroy);
	CPhysicItem::OnH_B_Independent(just_before_destroy);
}

void CEatableItemObject::OnH_B_Chield( )
{
	CPhysicItem::OnH_B_Chield( );
	CEatableItem::OnH_B_Chield( );
}

void CEatableItemObject::OnH_A_Chield( )
{
	CPhysicItem::OnH_A_Chield( );
	CEatableItem::OnH_A_Chield( );
}

void CEatableItemObject::UpdateCL( )
{
	CPhysicItem::UpdateCL( );
	CEatableItem::UpdateCL( );
}

void CEatableItemObject::OnEvent(CNetPacket& P, u16 type)
{
	CPhysicItem::OnEvent(P, type);
	CEatableItem::OnEvent(P, type);
}

BOOL CEatableItemObject::net_Spawn(CSE_Abstract* DC)
{
	BOOL res = CPhysicItem::net_Spawn(DC);
	CEatableItem::net_Spawn(DC);
	return res;
}

void CEatableItemObject::net_Destroy( )
{
	CEatableItem::net_Destroy( );
	CPhysicItem::net_Destroy( );
}

void CEatableItemObject::net_Import(CNetPacket& P)
{
	CEatableItem::net_Import(P);
}

void CEatableItemObject::net_Export(CNetPacket& P)
{
	CEatableItem::net_Export(P);
}

void CEatableItemObject::save(CNetPacket& packet)
{
	CPhysicItem::save(packet);
	CEatableItem::save(packet);
}

void CEatableItemObject::load(IReader& packet)
{
	CPhysicItem::load(packet);
	CEatableItem::load(packet);
}

void CEatableItemObject::renderable_Render( )
{
	CPhysicItem::renderable_Render( );
	CEatableItem::renderable_Render( );
}

void CEatableItemObject::reload(pcstr section)
{
	CPhysicItem::reload(section);
	CEatableItem::reload(section);
}

void CEatableItemObject::reinit( )
{
	CEatableItem::reinit( );
	CPhysicItem::reinit( );
}

void CEatableItemObject::activate_physic_shell( )
{
	CEatableItem::activate_physic_shell( );
}

void CEatableItemObject::on_activate_physic_shell( )
{
	CPhysicItem::activate_physic_shell( );
}

void CEatableItemObject::make_Interpolation( )
{
	CEatableItem::make_Interpolation( );
}

void CEatableItemObject::PH_B_CrPr( )
{
	CEatableItem::PH_B_CrPr( );
}

void CEatableItemObject::PH_I_CrPr( )
{
	CEatableItem::PH_I_CrPr( );
}

#ifdef DEBUG
void CEatableItemObject::PH_Ch_CrPr( )
{
	CEatableItem::PH_Ch_CrPr( );
}
#endif

void CEatableItemObject::PH_A_CrPr( )
{
	CEatableItem::PH_A_CrPr( );
}

#ifdef DEBUG
void CEatableItemObject::OnRender( )
{
	CEatableItem::OnRender( );
}
#endif

bool CEatableItemObject::NeedToDestroyObject( ) const
{
	return CInventoryItem::NeedToDestroyObject( );
}

u32 CEatableItemObject::ef_weapon_type( ) const
{
	return 0;
}

bool CEatableItemObject::Useful( ) const
{
	return CEatableItem::Useful( );
}
