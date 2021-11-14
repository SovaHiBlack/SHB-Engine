//	Module 		: EatableItemObject.cpp
//	Description : Eatable item object implementation

#include "stdafx.h"

#include "EatableItemObject.h"

CEatableItemObject::CEatableItemObject( )
{ }

CEatableItemObject::~CEatableItemObject( )
{ }

DLL_Pure* CEatableItemObject::_construct( )
{
	CEatableItem::_construct( );
	CPHItem::_construct( );
	return this;
}

void CEatableItemObject::Load(const char* section)
{
	CPHItem::Load(section);
	CEatableItem::Load(section);
}

void CEatableItemObject::Hit(SHit* pHDS)
{
	CPHItem::Hit(pHDS);
	CEatableItem::Hit(pHDS);
}

void CEatableItemObject::OnH_A_Independent( )
{
	CEatableItem::OnH_A_Independent( );
	CPHItem::OnH_A_Independent( );
}

void CEatableItemObject::OnH_B_Independent(bool just_before_destroy)
{
	CEatableItem::OnH_B_Independent(just_before_destroy);
	CPHItem::OnH_B_Independent(just_before_destroy);
}

void CEatableItemObject::OnH_B_Chield( )
{
	CPHItem::OnH_B_Chield( );
	CEatableItem::OnH_B_Chield( );
}

void CEatableItemObject::OnH_A_Chield( )
{
	CPHItem::OnH_A_Chield( );
	CEatableItem::OnH_A_Chield( );
}

void CEatableItemObject::UpdateCL( )
{
	CPHItem::UpdateCL( );
	CEatableItem::UpdateCL( );
}

void CEatableItemObject::OnEvent(CNetPacket& P, unsigned short type)
{
	CPHItem::OnEvent(P, type);
	CEatableItem::OnEvent(P, type);
}

BOOL CEatableItemObject::net_Spawn(CSE_Abstract* DC)
{
	BOOL res = CPHItem::net_Spawn(DC);
	CEatableItem::net_Spawn(DC);
	return res;
}

void CEatableItemObject::net_Destroy( )
{
	CEatableItem::net_Destroy( );
	CPHItem::net_Destroy( );
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
	CPHItem::save(packet);
	CEatableItem::save(packet);
}

void CEatableItemObject::load(IReader& packet)
{
	CPHItem::load(packet);
	CEatableItem::load(packet);
}

void CEatableItemObject::renderable_Render( )
{
	CPHItem::renderable_Render( );
	CEatableItem::renderable_Render( );
}

void CEatableItemObject::reload(const char* section)
{
	CPHItem::reload(section);
	CEatableItem::reload(section);
}

void CEatableItemObject::reinit( )
{
	CEatableItem::reinit( );
	CPHItem::reinit( );
}

void CEatableItemObject::activate_physic_shell( )
{
	CEatableItem::activate_physic_shell( );
}

void CEatableItemObject::on_activate_physic_shell( )
{
	CPHItem::activate_physic_shell( );
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
#endif // def DEBUG

void CEatableItemObject::PH_A_CrPr( )
{
	CEatableItem::PH_A_CrPr( );
}

#ifdef DEBUG
void CEatableItemObject::OnRender( )
{
	CEatableItem::OnRender( );
}
#endif // def DEBUG

bool CEatableItemObject::NeedToDestroyObject( ) const
{
	return CInventoryItem::NeedToDestroyObject( );
}

unsigned int CEatableItemObject::ef_weapon_type( ) const
{
	return 0;
}

bool CEatableItemObject::Useful( ) const
{
	return CEatableItem::Useful( );
}
