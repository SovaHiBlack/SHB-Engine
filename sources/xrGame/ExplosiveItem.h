//////////////////////////////////////////////////////////////////////
// ExplosiveItem.h: ����� ��� ���� ������� ���������� ��� 
//					��������� ��������� ����� (��������,
//					������ � ����� � �.�.)
//////////////////////////////////////////////////////////////////////

#pragma once

#include "Explosive.h"
#include "InventoryItemObject.h"
#include "DelayedActionFuse.h"
class CExplosiveItem: 
			public CInventoryItemObject	,
			public CDelayedActionFuse	,
			public CExplosive
{
private:
	typedef CInventoryItemObject inherited;

public:
	CExplosiveItem(void);
	virtual ~CExplosiveItem(void);

	virtual void				Load					(pcstr section)							;
	virtual BOOL				net_Spawn				(CSE_Abstract* DC)							{return CInventoryItemObject::net_Spawn(DC);}
	virtual void				net_Destroy				()											;
	virtual void				net_Export				(CNetPacket& P)								{CInventoryItemObject::net_Export(P);}
	virtual void				net_Import				(CNetPacket& P)								{CInventoryItemObject::net_Import(P);}
	virtual void				net_Relcase				(CObject* O )								;
	virtual CGameObject			*cast_game_object		()											{return this;}
	virtual CExplosive*			cast_explosive			()											{return this;}
	virtual IDamageSource*		cast_IDamageSource		()											{return CExplosive::cast_IDamageSource();}
	virtual void				GetRayExplosionSourcePos(fVector3& pos);
	virtual void				ActivateExplosionBox(const fVector3& size, fVector3& in_out_pos);
	virtual void				OnEvent					(CNetPacket& P, u16 type)					;
	virtual	void				Hit						(SHit* pHDS)								;
	virtual void				shedule_Update			(u32 dt)									;
	virtual bool				shedule_Needed			();

	virtual void				UpdateCL				()											;
	virtual void				renderable_Render		()											; 
	virtual void				ChangeCondition			(f32 fDeltaCondition)						{CInventoryItem::ChangeCondition(fDeltaCondition);};
	virtual void				StartTimerEffects		()											;

};