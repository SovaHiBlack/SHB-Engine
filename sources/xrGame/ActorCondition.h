// ActorCondition.h: класс состояния игрока
//

#pragma once

#include "EntityCondition.h"
#include "actor_defs.h"

template <typename _return_type>
class CScriptCallbackEx;



class CActor;
//class CUIActorSleepVideoPlayer;

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
	Flags16											m_condition_flags;
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

	virtual void 		ChangeAlcohol				(F32 value);
	virtual void 		ChangeSatiety				(F32 value);

	// хромание при потере сил и здоровья
	virtual	bool		IsLimping					() const;
	virtual bool		IsCantWalk					() const;
	virtual bool		IsCantWalkWeight			();
	virtual bool		IsCantSprint				() const;

			void		ConditionJump				(F32 weight);
			void		ConditionWalk				(F32 weight, bool accel, bool sprint);
			void		ConditionStand				(F32 weight);
			
			F32	xr_stdcall	GetAlcohol			()	{return m_fAlcohol;}
			F32	xr_stdcall	GetPsy				()	{return 1.0f-GetPsyHealth();}
			F32				GetSatiety			()  {return m_fSatiety;}

public:
	IC		CActor		&object						() const
	{
		VERIFY			(m_object);
		return			(*m_object);
	}
	virtual void			save					(NET_Packet &output_packet);
	virtual void			load					(IReader &input_packet);

protected:
	F32 m_fAlcohol;
	F32 m_fV_Alcohol;
//--
	F32 m_fSatiety;
	F32 m_fV_Satiety;
	F32 m_fV_SatietyPower;
	F32 m_fV_SatietyHealth;
//--
	F32 m_fPowerLeakSpeed;

	F32 m_fJumpPower;
	F32 m_fStandPower;
	F32 m_fWalkPower;
	F32 m_fJumpWeightPower;
	F32 m_fWalkWeightPower;
	F32 m_fOverweightWalkK;
	F32 m_fOverweightJumpK;
	F32 m_fAccelK;
	F32 m_fSprintK;
	
	F32	m_MaxWalkWeight;

	mutable bool m_bLimping;
	mutable bool m_bCantWalk;
	mutable bool m_bCantSprint;

	//порог силы и здоровья меньше которого актер начинает хромать
	F32 m_fLimpingPowerBegin;
	F32 m_fLimpingPowerEnd;
	F32 m_fCantWalkPowerBegin;
	F32 m_fCantWalkPowerEnd;

	F32 m_fCantSprintPowerBegin;
	F32 m_fCantSprintPowerEnd;

	F32 m_fLimpingHealthBegin;
	F32 m_fLimpingHealthEnd;
};
