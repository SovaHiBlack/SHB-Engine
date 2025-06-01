////////////////////////////////////////////////////////////////////////////
//	Module 		: object_property_evaluators.h
//	Created 	: 12.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Object property evaluators
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "property_evaluator_const.h"
#include "property_evaluator_member.h"

class CGameObject;
class CStalker;
class CWeapon;
class CMissile;
class CFoodItem;
class CWeaponMagazined;

typedef CPropertyEvaluatorMember<CStalker> CObjectPropertyEvaluatorMember;

//////////////////////////////////////////////////////////////////////////
// CObjectPropertyEvaluatorBase
//////////////////////////////////////////////////////////////////////////

template <typename _item_type>
class CObjectPropertyEvaluatorBase : public CPropertyEvaluator<CStalker> {
protected:
	typedef CPropertyEvaluator<CStalker> inherited;
	_item_type		*m_item;

public:
	IC					CObjectPropertyEvaluatorBase(_item_type* item, CStalker* owner);
	IC		CStalker& object( ) const;
};

typedef CObjectPropertyEvaluatorBase<CGameObject>	CSObjectPropertyEvaluatorBase;
typedef CPropertyEvaluatorConst<CStalker>		CObjectPropertyEvaluatorConst;

//////////////////////////////////////////////////////////////////////////
// CObjectPropertyEvaluatorState
//////////////////////////////////////////////////////////////////////////

class CObjectPropertyEvaluatorState : public CObjectPropertyEvaluatorBase<CWeapon> {
protected:
	typedef CObjectPropertyEvaluatorBase<CWeapon> inherited;
	u32						m_state;
	bool					m_equality;

public:
	CObjectPropertyEvaluatorState(CWeapon* item, CStalker* owner, u32 state, bool equality = true);
	virtual _value_type		evaluate						();
};

class CObjectPropertyEvaluatorWeaponHidden : public CObjectPropertyEvaluatorBase<CWeapon> {
protected:
	typedef CObjectPropertyEvaluatorBase<CWeapon> inherited;
public:
	CObjectPropertyEvaluatorWeaponHidden(CWeapon* item, CStalker* owner);
	virtual _value_type		evaluate						();
};

//////////////////////////////////////////////////////////////////////////
// CObjectPropertyEvaluatorAmmo
//////////////////////////////////////////////////////////////////////////

class CObjectPropertyEvaluatorAmmo : public CObjectPropertyEvaluatorBase<CWeapon> {
protected:
	typedef CObjectPropertyEvaluatorBase<CWeapon> inherited;
	u32						m_ammo_type;

public:
	CObjectPropertyEvaluatorAmmo(CWeapon* item, CStalker* owner, u32 ammo_type);
	virtual _value_type		evaluate						();
};

//////////////////////////////////////////////////////////////////////////
// CObjectPropertyEvaluatorEmpty
//////////////////////////////////////////////////////////////////////////

class CObjectPropertyEvaluatorEmpty : public CObjectPropertyEvaluatorBase<CWeapon> {
protected:
	typedef CObjectPropertyEvaluatorBase<CWeapon> inherited;
	u32						m_ammo_type;

public:
	CObjectPropertyEvaluatorEmpty(CWeapon* item, CStalker* owner, u32 ammo_type);
	virtual _value_type		evaluate						();
};

//////////////////////////////////////////////////////////////////////////
// CObjectPropertyEvaluatorFull
//////////////////////////////////////////////////////////////////////////

class CObjectPropertyEvaluatorFull : public CObjectPropertyEvaluatorBase<CWeapon> {
protected:
	typedef CObjectPropertyEvaluatorBase<CWeapon> inherited;
	u32						m_ammo_type;

public:
	CObjectPropertyEvaluatorFull(CWeapon* item, CStalker* owner, u32 ammo_type);
	virtual _value_type		evaluate						();
};

//////////////////////////////////////////////////////////////////////////
// CObjectPropertyEvaluatorReady
//////////////////////////////////////////////////////////////////////////

class CObjectPropertyEvaluatorReady : public CObjectPropertyEvaluatorBase<CWeapon> {
protected:
	typedef CObjectPropertyEvaluatorBase<CWeapon> inherited;
	u32						m_ammo_type;

public:
	CObjectPropertyEvaluatorReady(CWeapon* item, CStalker* owner, u32 ammo_type);
	virtual _value_type		evaluate						();
};

//////////////////////////////////////////////////////////////////////////
// CObjectPropertyEvaluatorQueue
//////////////////////////////////////////////////////////////////////////

class CObjectPropertyEvaluatorQueue : public CObjectPropertyEvaluatorBase<CWeapon> {
protected:
	typedef CObjectPropertyEvaluatorBase<CWeapon> inherited;

protected:
	u32						m_type;
	CWeaponMagazined		*m_magazined;

public:
	CObjectPropertyEvaluatorQueue(CWeapon* item, CStalker* owner, u32 type);
	virtual _value_type		evaluate						();
};

//////////////////////////////////////////////////////////////////////////
// CObjectPropertyEvaluatorNoItems
//////////////////////////////////////////////////////////////////////////

class CObjectPropertyEvaluatorNoItems : public CPropertyEvaluator<CStalker> {
protected:
	typedef CPropertyEvaluator<CStalker> inherited;

public:
	CObjectPropertyEvaluatorNoItems(CStalker* owner);
	virtual _value_type	evaluate						();
	IC		CStalker& object( ) const;
};

//////////////////////////////////////////////////////////////////////////
// CObjectPropertyEvaluatorMissile
//////////////////////////////////////////////////////////////////////////

class CObjectPropertyEvaluatorMissile : public CObjectPropertyEvaluatorBase<CMissile> {
protected:
	typedef CObjectPropertyEvaluatorBase<CMissile> inherited;
	u32						m_state;
	bool					m_equality;

public:
	CObjectPropertyEvaluatorMissile(CMissile* item, CStalker* owner, u32 state, bool equality = true);
	virtual _value_type		evaluate						();
};

#include "object_property_evaluators_inline.h"
