//	Module 		: InventoryItemObject.cpp
//	Description : Inventory item object implementation

#include "stdafx.h"

#include "InventoryItemObject.h"

CInventoryItemObject::CInventoryItemObject( )
{ }

CInventoryItemObject::~CInventoryItemObject( )
{ }

DLL_Pure* CInventoryItemObject::_construct( )
{
	CInventoryItem::_construct( );
	CPHItem::_construct( );
	return this;
}

void CInventoryItemObject::Load(const char* section)
{
	CPHItem::Load(section);
	CInventoryItem::Load(section);
}

const char* CInventoryItemObject::Name( )
{
	return CInventoryItem::Name( );
}

const char* CInventoryItemObject::NameShort( )
{
	return CInventoryItem::NameShort( );
}

void CInventoryItemObject::Hit(SHit* pHDS)
{
	CPHItem::Hit(pHDS);
	CInventoryItem::Hit(pHDS);
}

void CInventoryItemObject::OnH_B_Independent(bool just_before_destroy)
{
	CInventoryItem::OnH_B_Independent(just_before_destroy);
	CPHItem::OnH_B_Independent(just_before_destroy);
}

void CInventoryItemObject::OnH_A_Independent( )
{
	CInventoryItem::OnH_A_Independent( );
	CPHItem::OnH_A_Independent( );
}

void CInventoryItemObject::OnH_B_Chield( )
{
	CPHItem::OnH_B_Chield( );
	CInventoryItem::OnH_B_Chield( );
}

void CInventoryItemObject::OnH_A_Chield( )
{
	CPHItem::OnH_A_Chield( );
	CInventoryItem::OnH_A_Chield( );
}

void CInventoryItemObject::UpdateCL( )
{
	CPHItem::UpdateCL( );
	CInventoryItem::UpdateCL( );
}

void CInventoryItemObject::OnEvent(CNetPacket& P, U16 type)
{
	CPHItem::OnEvent(P, type);
	CInventoryItem::OnEvent(P, type);
}

BOOL CInventoryItemObject::net_Spawn(CSE_Abstract* DC)
{
	BOOL res = CPHItem::net_Spawn(DC);
	CInventoryItem::net_Spawn(DC);
	return res;
}

void CInventoryItemObject::net_Destroy( )
{
	CInventoryItem::net_Destroy( );
	CPHItem::net_Destroy( );
}

void CInventoryItemObject::net_Import(CNetPacket& P)
{
	CInventoryItem::net_Import(P);
}

void CInventoryItemObject::net_Export(CNetPacket& P)
{
	CInventoryItem::net_Export(P);
}

void CInventoryItemObject::save(CNetPacket& packet)
{
	CPHItem::save(packet);
	CInventoryItem::save(packet);
}

void CInventoryItemObject::load(IReader& packet)
{
	CPHItem::load(packet);
	CInventoryItem::load(packet);
}

void CInventoryItemObject::renderable_Render( )
{
	CPHItem::renderable_Render( );
	CInventoryItem::renderable_Render( );
}

void CInventoryItemObject::reload(const char* section)
{
	CPHItem::reload(section);
	CInventoryItem::reload(section);
}

void CInventoryItemObject::reinit( )
{
	CInventoryItem::reinit( );
	CPHItem::reinit( );
}

void CInventoryItemObject::activate_physic_shell( )
{
	CInventoryItem::activate_physic_shell( );
}

void CInventoryItemObject::on_activate_physic_shell( )
{
	CPHItem::activate_physic_shell( );
}

void CInventoryItemObject::make_Interpolation( )
{
	CInventoryItem::make_Interpolation( );
}

void CInventoryItemObject::PH_B_CrPr( )
{
	CInventoryItem::PH_B_CrPr( );
}

void CInventoryItemObject::PH_I_CrPr( )
{
	CInventoryItem::PH_I_CrPr( );
}

#ifdef DEBUG
void CInventoryItemObject::PH_Ch_CrPr( )
{
	CInventoryItem::PH_Ch_CrPr( );
}
#endif // def DEBUG

void CInventoryItemObject::PH_A_CrPr( )
{
	CInventoryItem::PH_A_CrPr( );
}

#ifdef DEBUG
void CInventoryItemObject::OnRender( )
{
	CInventoryItem::OnRender( );
}
#endif // def DEBUG

void CInventoryItemObject::modify_holder_params(float& range, float& fov) const
{
	CInventoryItem::modify_holder_params(range, fov);
}

unsigned int CInventoryItemObject::ef_weapon_type( ) const
{
	return 0;
}

bool CInventoryItemObject::NeedToDestroyObject( ) const
{
	return CInventoryItem::NeedToDestroyObject( );
}

bool CInventoryItemObject::Useful( ) const
{
	return CInventoryItem::Useful( );
}
