#pragma once

class CWound;
class CNetPacket;
class CEntityAlive;
class CLevel;

#include "HitImmunity.h"
#include "Hit.h"
#include "Level.h"

class CEntityConditionSimple
{
	f32					m_fHealth;
	f32					m_fHealthMax;
public:
	CEntityConditionSimple( );
	virtual					~CEntityConditionSimple( );

	IC f32				GetHealth( ) const
	{
		return m_fHealth;
	}
	IC f32 				GetMaxHealth( ) const
	{
		return m_fHealthMax;
	}
	IC f32& health( )
	{
		return	m_fHealth;
	}
	IC f32& max_health( )
	{
		return	m_fHealthMax;
	}
};

class CEntityCondition : public CEntityConditionSimple, public CHitImmunity
{
private:
	bool					m_use_limping_state;
	CEntityAlive* m_object;

public:
	CEntityCondition(CEntityAlive* object);
	virtual					~CEntityCondition(void);

	virtual void			LoadCondition(pcstr section);
	virtual void			remove_links(const CObject* object);

	virtual void			save(CNetPacket& output_packet);
	virtual void			load(IReader& input_packet);

	IC f32				GetPower( ) const
	{
		return m_fPower;
	}
	IC f32				GetRadiation( ) const
	{
		return m_fRadiation;
	}
	IC f32				GetPsyHealth( ) const
	{
		return m_fPsyHealth;
	}

	IC f32 				GetEntityMorale( ) const
	{
		return m_fEntityMorale;
	}

	IC f32 				GetHealthLost( ) const
	{
		return m_fHealthLost;
	}

	virtual bool 			IsLimping( ) const;

	virtual void			ChangeSatiety(f32 value)
	{ }
	void 					ChangeHealth(f32 value);
	void 					ChangePower(f32 value);
	void 					ChangeRadiation(f32 value);
	void 					ChangePsyHealth(f32 value);
	virtual void 			ChangeAlcohol(f32 value)
	{ }

	IC void					MaxPower( )
	{
		m_fPower = m_fPowerMax;
	}
	IC void					SetMaxPower(f32 val)
	{
		m_fPowerMax = val;
		clamp(m_fPowerMax, 0.1f, 1.0f);
	}
	IC f32				GetMaxPower( ) const
	{
		return m_fPowerMax;
	}

	void 					ChangeBleeding(f32 percent);

	void 					ChangeCircumspection(f32 value);
	void 					ChangeEntityMorale(f32 value);

	virtual CWound* ConditionHit(SHit* pHDS);
	//обновления состояния с течением времени
	virtual void			UpdateCondition( );
	void					UpdateWounds( );
	void					UpdateConditionTime( );
	IC void					SetConditionDeltaTime(f32 DeltaTime)
	{
		m_fDeltaTime = DeltaTime;
	}

//скорость потери крови из всех открытых ран 
	f32						BleedingSpeed( );

	CObject* GetWhoHitLastTime( )
	{
		return m_pWho;
	}
	u16						GetWhoHitLastTimeID( )
	{
		return m_iWhoID;
	}

	CWound* AddWound(f32 hit_power, ALife::EHitType hit_type, u16 element);

	IC void 				SetCanBeHarmedState(bool CanBeHarmed)
	{
		m_bCanBeHarmed = CanBeHarmed;
	}
	IC bool					CanBeHarmed( ) const
	{
		return (OnServer( ) && m_bCanBeHarmed);
	}

	void					ClearWounds( );

protected:
	void					UpdateHealth( );
	void					UpdatePower( );
	void					UpdateSatiety(f32 k = 1.0f);
	void					UpdateRadiation(f32 k = 1.0f);
	void					UpdatePsyHealth(f32 k = 1.0f);

	void					UpdateEntityMorale( );

	//изменение силы хита в зависимости от надетого костюма
	//(только для InventoryOwner)
	f32					HitOutfitEffect(f32 hit_power, ALife::EHitType hit_type, s16 element, f32 AP);
	//изменение потери сил в зависимости от надетого костюма
	f32					HitPowerEffect(f32 power_loss);

	//для подсчета состояния открытых ран,
	//запоминается кость куда был нанесен хит
	//и скорость потери крови из раны
	DEFINE_VECTOR(CWound*, WOUND_VECTOR, WOUND_VECTOR_IT);
	WOUND_VECTOR			m_WoundVector;
	//очистка массива ран

	//все величины от 0 до 1			
	f32 m_fPower;					//сила
	f32 m_fRadiation;				//доза радиактивного облучения
	f32 m_fPsyHealth;				//здоровье

	f32 m_fEntityMorale;			//мораль

	//максимальные величины
	//	f32 m_fSatietyMax;
	f32 m_fPowerMax;
	f32 m_fRadiationMax;
	f32 m_fPsyHealthMax;

	f32 m_fEntityMoraleMax;

	//величины изменения параметров на каждом обновлении
	f32 m_fDeltaHealth;
	f32 m_fDeltaPower;
	f32 m_fDeltaRadiation;
	f32 m_fDeltaPsyHealth;

	f32 m_fDeltaCircumspection;
	f32 m_fDeltaEntityMorale;

	struct SConditionChangeV
	{
		f32			m_fV_Radiation;
		f32			m_fV_PsyHealth;
		f32			m_fV_Circumspection;
		f32			m_fV_EntityMorale;
		f32			m_fV_RadiationHealth;
		f32			m_fV_Bleeding;
		f32			m_fV_WoundIncarnation;
		f32			m_fV_HealthRestore;
		void			load(pcstr sect, pcstr prefix);
	};

	SConditionChangeV m_change_v;

	f32				m_fMinWoundSize;
	bool				m_bIsBleeding;

	//части хита, затрачиваемые на уменьшение здоровья и силы
	f32				m_fHealthHitPart;
	f32				m_fPowerHitPart;

	//потеря здоровья от последнего хита
	f32				m_fHealthLost;

	//для отслеживания времени 
	u64					m_iLastTimeCalled;
	f32				m_fDeltaTime;
	//кто нанес последний хит
	CObject* m_pWho;
	u16					m_iWhoID;

	//для передачи параметров из DamageManager
	f32				m_fHitBoneScale;
	f32				m_fWoundBoneScale;

	f32				m_limping_threshold;

	bool				m_bTimeValid;
	bool				m_bCanBeHarmed;

public:
	virtual void					reinit( );

	IC const	f32				fdelta_time( ) const
	{
		return		(m_fDeltaTime);
	}
	IC const	WOUND_VECTOR& wounds( ) const
	{
		return		(m_WoundVector);
	}
	IC f32& radiation( )
	{
		return		(m_fRadiation);
	}
	IC f32& hit_bone_scale( )
	{
		return		(m_fHitBoneScale);
	}
	IC f32& wound_bone_scale( )
	{
		return		(m_fWoundBoneScale);
	}
};
