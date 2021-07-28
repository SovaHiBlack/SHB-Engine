#pragma once

class CWound;
class CNetPacket;
class CEntityAlive;
class CLevel;

#include "hit_immunity.h"
#include "Hit.h"
#include "Level.h"

class CEntityConditionSimple
{
	float					m_fHealth;
	float					m_fHealthMax;

public:
	CEntityConditionSimple( );
	virtual					~CEntityConditionSimple( );

	inline float				GetHealth( ) const
	{
		return m_fHealth;
	}
	inline float 				GetMaxHealth( ) const
	{
		return m_fHealthMax;
	}
	inline float& health( )
	{
		return m_fHealth;
	}
	inline float& max_health( )
	{
		return m_fHealthMax;
	}
};

class CEntityCondition : public CEntityConditionSimple, public CHitImmunity
{
private:
	bool					m_use_limping_state;
	CEntityAlive* m_object;

public:
	CEntityCondition(CEntityAlive* object);
	virtual					~CEntityCondition( );

	virtual void			LoadCondition(const char* section);
	virtual void			remove_links(const CObject* object);

	virtual void			save(CNetPacket& output_packet);
	virtual void			load(IReader& input_packet);

	inline float				GetPower( ) const
	{
		return m_fPower;
	}
	inline float				GetRadiation( ) const
	{
		return m_fRadiation;
	}
	inline float				GetPsyHealth( ) const
	{
		return m_fPsyHealth;
	}
	inline float 				GetEntityMorale( ) const
	{
		return m_fEntityMorale;
	}
	inline float 				GetHealthLost( ) const
	{
		return m_fHealthLost;
	}

	virtual bool 			IsLimping( ) const;

	virtual void			ChangeSatiety(float value)
	{ }
	void 					ChangeHealth(float value);
	void 					ChangePower(float value);
	void 					ChangeRadiation(float value);
	void 					ChangePsyHealth(float value);
	virtual void 			ChangeAlcohol(float value)
	{ }

	inline void					MaxPower( )
	{
		m_fPower = m_fPowerMax;
	}
	inline void					SetMaxPower(float val)
	{
		m_fPowerMax = val;
		clamp(m_fPowerMax, 0.1f, 1.0f);
	}
	inline float				GetMaxPower( ) const
	{
		return m_fPowerMax;
	}

	void 					ChangeBleeding(float percent);
	void 					ChangeCircumspection(float value);
	void 					ChangeEntityMorale(float value);

	virtual CWound* ConditionHit(SHit* pHDS);
	//обновления состояния с течением времени
	virtual void			UpdateCondition( );
	void					UpdateWounds( );
	void					UpdateConditionTime( );
	inline void					SetConditionDeltaTime(float DeltaTime)
	{
		m_fDeltaTime = DeltaTime;
	}

	//скорость потери крови из всех открытых ран 
	float					BleedingSpeed( );

	CObject* GetWhoHitLastTime( )
	{
		return m_pWho;
	}
	U16						GetWhoHitLastTimeID( )
	{
		return m_iWhoID;
	}

	CWound* AddWound(float hit_power, ALife::EHitType hit_type, U16 element);

	inline void 				SetCanBeHarmedState(bool CanBeHarmed)
	{
		m_bCanBeHarmed = CanBeHarmed;
	}
	inline bool					CanBeHarmed( ) const
	{
		return (OnServer( ) && m_bCanBeHarmed);
	}

	void					ClearWounds( );

protected:
	void					UpdateHealth( );
	void					UpdatePower( );
	void					UpdateRadiation(float k = 1.0f);
	void					UpdatePsyHealth(float k = 1.0f);

	void					UpdateEntityMorale( );

	//изменение силы хита в зависимости от надетого костюма (только для InventoryOwner)
	float					HitOutfitEffect(float hit_power, ALife::EHitType hit_type, S16 element, float AP);
	//изменение потери сил в зависимости от надетого костюма
	float					HitPowerEffect(float power_loss);

	//для подсчета состояния открытых ран, запоминается кость куда был нанесен хит и скорость потери крови из раны
	using WOUND_VECTOR = xr_vector<CWound*>;
	using WOUND_VECTOR_IT = WOUND_VECTOR::iterator;
	WOUND_VECTOR			m_WoundVector;

	//все величины от 0 до 1
	float m_fPower;					//сила
	float m_fRadiation;				//доза радиактивного облучения
	float m_fPsyHealth;				//здоровье
	float m_fEntityMorale;			//мораль

	//максимальные величины
	float m_fPowerMax;
	float m_fRadiationMax;
	float m_fPsyHealthMax;
	float m_fEntityMoraleMax;

	//величины изменения параметров на каждом обновлении
	float m_fDeltaHealth;
	float m_fDeltaPower;
	float m_fDeltaRadiation;
	float m_fDeltaPsyHealth;
	float m_fDeltaCircumspection;
	float m_fDeltaEntityMorale;

	struct SConditionChangeV
	{
		float			m_fV_Radiation;
		float			m_fV_PsyHealth;
		float			m_fV_EntityMorale;
		float			m_fV_RadiationHealth;
		float			m_fV_Bleeding;
		float			m_fV_WoundIncarnation;
		float			m_fV_HealthRestore;
		void			load(const char* sect, const char* prefix);
	};

	SConditionChangeV	m_change_v;

	float				m_fMinWoundSize;
	bool				m_bIsBleeding;

	//части хита, затрачиваемые на уменьшение здоровья и силы
	float				m_fHealthHitPart;
	float				m_fPowerHitPart;

	//потеря здоровья от последнего хита
	float				m_fHealthLost;

	//для отслеживания времени
	U64					m_iLastTimeCalled;
	float				m_fDeltaTime;

	//кто нанес последний хит
	CObject* m_pWho;
	U16					m_iWhoID;

	//для передачи параметров из DamageManager
	float				m_fHitBoneScale;
	float				m_fWoundBoneScale;

	float				m_limping_threshold;

	bool				m_bTimeValid;
	bool				m_bCanBeHarmed;

public:
	virtual void		reinit( );

	inline const float		fdelta_time( ) const
	{
		return m_fDeltaTime;
	}
	inline const WOUND_VECTOR& wounds( ) const
	{
		return m_WoundVector;
	}
	inline float& radiation( )
	{
		return m_fRadiation;
	}
	inline float& hit_bone_scale( )
	{
		return m_fHitBoneScale;
	}
	inline float& wound_bone_scale( )
	{
		return m_fWoundBoneScale;
	}
};
