// ActorCondition.h: класс состояния игрока
//

#pragma once

#include "EntityCondition.h"
#include "actor_defs.h"

template <typename _return_type>
class CScriptCallbackEx;

class CActor;

class CActorCondition: public CEntityCondition {
private:
	typedef CEntityCondition inherited;
	enum {	eCriticalPowerReached			=(1<<0),
			eCriticalMaxPowerReached		=(1<<1),
			eCriticalBleedingSpeed			=(1<<2),
			eCriticalSatietyReached			=(1<<3),
			eCriticalRadiationReached		=(1<<4),
			eWeaponJammedReached			=(1<<5),
			ePhyHealthMinReached			=(1<<6),
			eCantWalkWeight					=(1<<7),
			};
	flags16											m_condition_flags;
private:
	CActor*											m_object;
	void				UpdateTutorialThresholds	();
	void 				UpdateSatiety				();
public:
						CActorCondition				(CActor *object);
	virtual				~CActorCondition			(void);

	virtual void		LoadCondition				(pcstr section);
	virtual void		reinit						();

	virtual CWound*		ConditionHit				(SHit* pHDS);
	virtual void		UpdateCondition				();

	virtual void 		ChangeAlcohol				(f32 value);
	virtual void 		ChangeSatiety				(f32 value);

	// хромание при потере сил и здоровья
	virtual	bool		IsLimping					() const;
	virtual bool		IsCantWalk					() const;
	virtual bool		IsCantWalkWeight			();
	virtual bool		IsCantSprint				() const;

			void		ConditionJump				(f32 weight);
			void		ConditionWalk				(f32 weight, bool accel, bool sprint);
			void		ConditionStand				(f32 weight);
			
			f32	xr_stdcall	GetAlcohol			()	{return m_fAlcohol;}
			f32	xr_stdcall	GetPsy				()	{return 1.0f-GetPsyHealth();}
			f32				GetSatiety			()  {return m_fSatiety;}

public:
	IC		CActor		&object						() const
	{
		VERIFY			(m_object);
		return			(*m_object);
	}
	virtual void			save					(NET_Packet &output_packet);
	virtual void			load					(IReader &input_packet);

protected:
	f32 m_fAlcohol;
	f32 m_fV_Alcohol;
//--
	f32 m_fSatiety;
	f32 m_fV_Satiety;
	f32 m_fV_SatietyPower;
	f32 m_fV_SatietyHealth;
//--
	f32 m_fPowerLeakSpeed;

	f32 m_fJumpPower;
	f32 m_fStandPower;
	f32 m_fWalkPower;
	f32 m_fJumpWeightPower;
	f32 m_fWalkWeightPower;
	f32 m_fOverweightWalkK;
	f32 m_fOverweightJumpK;
	f32 m_fAccelK;
	f32 m_fSprintK;
	
	f32	m_MaxWalkWeight;

	mutable bool m_bLimping;
	mutable bool m_bCantWalk;
	mutable bool m_bCantSprint;

	//порог силы и здоровья меньше которого актер начинает хромать
	f32 m_fLimpingPowerBegin;
	f32 m_fLimpingPowerEnd;
	f32 m_fCantWalkPowerBegin;
	f32 m_fCantWalkPowerEnd;

	f32 m_fCantSprintPowerBegin;
	f32 m_fCantSprintPowerEnd;

	f32 m_fLimpingHealthBegin;
	f32 m_fLimpingHealthEnd;
};
