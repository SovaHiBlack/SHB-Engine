#include "stdafx.h"
#include "EntityCondition.h"
#include "InventoryOwner.h"
#include "CustomOutfit.h"
#include "Inventory.h"
#include "Wound.h"
#include "level.h"
#include "game_cl_base.h"
#include "EntityAlive.h"
#include "..\XR_3DA\SkeletonCustom.h"
#include "object_broker.h"

#define MAX_HEALTH 1.0f
#define MIN_HEALTH -0.01f

#define MAX_POWER 1.0f
#define MAX_RADIATION 1.0f
#define MAX_PSY_HEALTH 1.0f

CEntityConditionSimple::CEntityConditionSimple( )
{
	max_health( ) = MAX_HEALTH;
	health( ) = MAX_HEALTH;
}

CEntityConditionSimple::~CEntityConditionSimple( )
{ }

CEntityCondition::CEntityCondition(CEntityAlive* object) : CEntityConditionSimple( )
{
	VERIFY(object);

	m_object = object;

	m_use_limping_state = false;
	m_iLastTimeCalled = 0;
	m_bTimeValid = false;

	m_fPowerMax = MAX_POWER;
	m_fRadiationMax = MAX_RADIATION;
	m_fPsyHealthMax = MAX_PSY_HEALTH;
	m_fEntityMorale = m_fEntityMoraleMax = 1.0f;

	m_fPower = MAX_POWER;
	m_fRadiation = 0.0f;
	m_fPsyHealth = MAX_PSY_HEALTH;

	m_fMinWoundSize = 0.00001f;


	m_fHealthHitPart = 1.0f;
	m_fPowerHitPart = 0.5f;

	m_fDeltaHealth = 0.0f;
	m_fDeltaPower = 0.0f;
	m_fDeltaRadiation = 0.0f;
	m_fDeltaPsyHealth = 0.0f;

	m_fHealthLost = 0.0f;
	m_pWho = NULL;
	m_iWhoID = 0;

	m_WoundVector.clear( );

	m_fHitBoneScale = 1.0f;
	m_fWoundBoneScale = 1.0f;

	m_bIsBleeding = false;
	m_bCanBeHarmed = true;
}

CEntityCondition::~CEntityCondition( )
{
	ClearWounds( );
}

void CEntityCondition::ClearWounds( )
{
	for (WOUND_VECTOR_IT it = m_WoundVector.begin( ); m_WoundVector.end( ) != it; ++it)
	{
		xr_delete(*it);
	}

	m_WoundVector.clear( );

	m_bIsBleeding = false;
}

void CEntityCondition::LoadCondition(pcstr entity_section)
{
	pcstr section = READ_IF_EXISTS(pSettings, r_string, entity_section, "condition_sect", entity_section);

	m_change_v.load(section, "");

	m_fMinWoundSize = pSettings->r_float(section, "min_wound_size");
	m_fHealthHitPart = pSettings->r_float(section, "health_hit_part");
	m_fPowerHitPart = pSettings->r_float(section, "power_hit_part");

	m_use_limping_state = !!(READ_IF_EXISTS(pSettings, r_bool, section, "use_limping_state", FALSE));
	m_limping_threshold = READ_IF_EXISTS(pSettings, r_float, section, "limping_threshold", .5f);
}

void CEntityCondition::reinit( )
{
	m_iLastTimeCalled = 0;
	m_bTimeValid = false;

	max_health( ) = MAX_HEALTH;
	m_fPowerMax = MAX_POWER;
	m_fRadiationMax = MAX_RADIATION;
	m_fPsyHealthMax = MAX_PSY_HEALTH;

	m_fEntityMorale = m_fEntityMoraleMax = 1.0f;

	health( ) = MAX_HEALTH;
	m_fPower = MAX_POWER;
	m_fRadiation = 0.0f;
	m_fPsyHealth = MAX_PSY_HEALTH;

	m_fDeltaHealth = 0.0f;
	m_fDeltaPower = 0.0f;
	m_fDeltaRadiation = 0.0f;
	m_fDeltaCircumspection = 0.0f;
	m_fDeltaEntityMorale = 0.0f;
	m_fDeltaPsyHealth = 0.0f;

	m_fHealthLost = 0.0f;
	m_pWho = NULL;
	m_iWhoID = NULL;

	ClearWounds( );
}

void CEntityCondition::ChangeHealth(f32 value)
{
	VERIFY(_valid(value));
	m_fDeltaHealth += (CanBeHarmed( ) || (value > 0.0f)) ? value : 0.0f;
}

void CEntityCondition::ChangePower(f32 value)
{
	m_fDeltaPower += value;
}

void CEntityCondition::ChangeRadiation(f32 value)
{
	m_fDeltaRadiation += value;
}

void CEntityCondition::ChangePsyHealth(f32 value)
{
	m_fDeltaPsyHealth += value;
}

void CEntityCondition::ChangeCircumspection(f32 value)
{
	m_fDeltaCircumspection += value;
}

void CEntityCondition::ChangeEntityMorale(f32 value)
{
	m_fDeltaEntityMorale += value;
}

void CEntityCondition::ChangeBleeding(f32 percent)
{
	//�������� ����
	for (WOUND_VECTOR_IT it = m_WoundVector.begin( ); m_WoundVector.end( ) != it; ++it)
	{
		(*it)->Incarnation(percent, m_fMinWoundSize);
		if (0 == (*it)->TotalSize( ))
		{
			(*it)->SetDestroy(true);
		}
	}
}

bool RemoveWoundPred(CWound* pWound)
{
	if (pWound->GetDestroy( ))
	{
		xr_delete(pWound);
		return true;
	}

	return false;
}

void  CEntityCondition::UpdateWounds( )
{
	//������ ��� �������� ���� �� ������
	m_WoundVector.erase(std::remove_if(m_WoundVector.begin( ), m_WoundVector.end( ), &RemoveWoundPred), m_WoundVector.end( ));
}

void CEntityCondition::UpdateConditionTime( )
{
	u64 _cur_time = Level( ).GetGameTime( );

	if (m_bTimeValid)
	{
		if (_cur_time > m_iLastTimeCalled)
		{
			f32 x = f32(_cur_time - m_iLastTimeCalled) / 1000.0f;
			SetConditionDeltaTime(x);

		}
		else
		{
			SetConditionDeltaTime(0.0f);
		}
	}
	else
	{
		SetConditionDeltaTime(0.0f);
		m_bTimeValid = true;

		m_fDeltaHealth = 0.0f;
		m_fDeltaPower = 0.0f;
		m_fDeltaRadiation = 0.0f;
		m_fDeltaCircumspection = 0.0f;
		m_fDeltaEntityMorale = 0.0f;
	}

	m_iLastTimeCalled = _cur_time;
}

//���������� ���������� � ����� �������� �������
void CEntityCondition::UpdateCondition( )
{
	if (GetHealth( ) <= 0.0f)
	{
		return;
	}
	//-----------------------------------------
	bool CriticalHealth = false;

	if (m_fDeltaHealth + GetHealth( ) <= 0.0f)
	{
		CriticalHealth = true;
	}

	//-----------------------------------------
	UpdateHealth( );
	//-----------------------------------------
	if (!CriticalHealth && m_fDeltaHealth + GetHealth( ) <= 0.0f)
	{
		CriticalHealth = true;
	}
	//-----------------------------------------
	UpdatePower( );
	UpdateRadiation( );
	//-----------------------------------------
	if (!CriticalHealth && m_fDeltaHealth + GetHealth( ) <= 0.0f)
	{
		CriticalHealth = true;
	}
	//-----------------------------------------
	UpdatePsyHealth( );

	UpdateEntityMorale( );

	health( ) += m_fDeltaHealth;
	m_fPower += m_fDeltaPower;
	m_fPsyHealth += m_fDeltaPsyHealth;
	m_fEntityMorale += m_fDeltaEntityMorale;
	m_fRadiation += m_fDeltaRadiation;

	m_fDeltaHealth = 0.0f;
	m_fDeltaPower = 0.0f;
	m_fDeltaRadiation = 0.0f;
	m_fDeltaPsyHealth = 0.0f;
	m_fDeltaCircumspection = 0.0f;
	m_fDeltaEntityMorale = 0.0f;

	clamp(health( ), MIN_HEALTH, max_health( ));
	clamp(m_fPower, 0.0f, m_fPowerMax);
	clamp(m_fRadiation, 0.0f, m_fRadiationMax);
	clamp(m_fEntityMorale, 0.0f, m_fEntityMoraleMax);
	clamp(m_fPsyHealth, 0.0f, m_fPsyHealthMax);
}

f32 CEntityCondition::HitOutfitEffect(f32 hit_power, ALife::EHitType hit_type, s16 element, f32 AP)
{
	CInventoryOwner* pInvOwner = smart_cast<CInventoryOwner*>(m_object);
	if (!pInvOwner)
	{
		return hit_power;
	}

	CCustomOutfit* pOutfit = (CCustomOutfit*)pInvOwner->inventory( ).m_slots[OUTFIT_SLOT].m_pIItem;
	if (!pOutfit)
	{
		return hit_power;
	}

	f32 new_hit_power = hit_power;

	if (hit_type == ALife::eHitTypeFireWound)
	{
		new_hit_power = pOutfit->HitThruArmour(hit_power, element, AP);
	}
	else
	{
		new_hit_power *= pOutfit->GetHitTypeProtection(hit_type, element);
	}

	//��������� ������������ �������
	pOutfit->Hit(hit_power, hit_type);

	return new_hit_power;
}

f32 CEntityCondition::HitPowerEffect(f32 power_loss)
{
	CInventoryOwner* pInvOwner = smart_cast<CInventoryOwner*>(m_object);
	if (!pInvOwner)
	{
		return power_loss;
	}

	CCustomOutfit* pOutfit = (CCustomOutfit*)pInvOwner->inventory( ).m_slots[OUTFIT_SLOT].m_pIItem;
	if (!pOutfit)
	{
		return power_loss;
	}

	f32 new_power_loss = power_loss * pOutfit->GetPowerLoss( );
	return new_power_loss;
}

CWound* CEntityCondition::AddWound(f32 hit_power, ALife::EHitType hit_type, u16 element)
{
	//������������ ����� �������� 64
	VERIFY(element < 64 || BI_NONE == element);

	//��������� ����� �� ������� ������� � ���� �����
	WOUND_VECTOR_IT it = m_WoundVector.begin( );
	for (; it != m_WoundVector.end( ); it++)
	{
		if ((*it)->GetBoneNum( ) == element)
		{
			break;
		}
	}

	CWound* pWound = NULL;

	//����� ����
	if (it == m_WoundVector.end( ))
	{
		pWound = xr_new<CWound>(element);
		pWound->AddHit(hit_power * ::Random.randF(0.5f, 1.5f), hit_type);
		m_WoundVector.push_back(pWound);
	}
	//������ 
	else
	{
		pWound = *it;
		pWound->AddHit(hit_power * ::Random.randF(0.5f, 1.5f), hit_type);
	}

	VERIFY(pWound);
	return pWound;
}

CWound* CEntityCondition::ConditionHit(SHit* pHDS)
{
	//��� ����� ��������� ���
	m_pWho = pHDS->who;
	m_iWhoID = (NULL != pHDS->who) ? pHDS->who->ID( ) : 0;

	f32 hit_power_org = pHDS->damage( );
	f32 hit_power = hit_power_org;
	hit_power = HitOutfitEffect(hit_power, pHDS->hit_type, pHDS->boneID, pHDS->ap);

	bool bAddWound = true;
	switch (pHDS->hit_type)
	{
		case ALife::eHitTypeTelepatic:
		{	// -------------------------------------------------
			// temp (till there is no death from psy hits)
			hit_power *= m_HitTypeK[pHDS->hit_type];
	/*
			m_fHealthLost = hit_power*m_fHealthHitPart*m_fHitBoneScale;
			m_fDeltaHealth -= CanBeHarmed() ? m_fHealthLost : 0;
			m_fDeltaPower -= hit_power*m_fPowerHitPart;
	*/
			// -------------------------------------------------

			hit_power *= m_HitTypeK[pHDS->hit_type];
			ChangePsyHealth(-hit_power);
			bAddWound = false;
		}
		break;
		case ALife::eHitTypeBurn:
		{
			hit_power *= m_HitTypeK[pHDS->hit_type];
			m_fHealthLost = hit_power * m_fHealthHitPart * m_fHitBoneScale;
			m_fDeltaHealth -= CanBeHarmed( ) ? m_fHealthLost : 0;
			m_fDeltaPower -= hit_power * m_fPowerHitPart;
			bAddWound = false;
		}
		break;
		case ALife::eHitTypeChemicalBurn:
		{
			hit_power *= m_HitTypeK[pHDS->hit_type];
		}
		break;
		case ALife::eHitTypeShock:
		{
			hit_power *= m_HitTypeK[pHDS->hit_type];
			m_fHealthLost = hit_power * m_fHealthHitPart;
			m_fDeltaHealth -= CanBeHarmed( ) ? m_fHealthLost : 0;
			m_fDeltaPower -= hit_power * m_fPowerHitPart;
			bAddWound = false;
		}
		break;
		case ALife::eHitTypeRadiation:
		{
			m_fDeltaRadiation += hit_power;
			return NULL;
		}
		break;
		case ALife::eHitTypeExplosion:
		case ALife::eHitTypeStrike:
		case ALife::eHitTypePhysicStrike:
		{
			hit_power *= m_HitTypeK[pHDS->hit_type];
			m_fHealthLost = hit_power * m_fHealthHitPart;
			m_fDeltaHealth -= CanBeHarmed( ) ? m_fHealthLost : 0;
			m_fDeltaPower -= hit_power * m_fPowerHitPart;
		}
		break;
		case ALife::eHitTypeFireWound:
		case ALife::eHitTypeWound:
		{
			hit_power *= m_HitTypeK[pHDS->hit_type];
			m_fHealthLost = hit_power * m_fHealthHitPart * m_fHitBoneScale;
			m_fDeltaHealth -= CanBeHarmed( ) ? m_fHealthLost : 0;
			m_fDeltaPower -= hit_power * m_fPowerHitPart;
		}
		break;
		default:
		{
			R_ASSERT2(0, "unknown hit type");
		}
		break;
	}

	if (bDebug)
	{
		Msg("%s hitted in %s with %f[%f]", m_object->Name( ), smart_cast<CKinematics*>(m_object->Visual( ))->LL_BoneName_dbg(pHDS->boneID), m_fHealthLost * 100.0f, hit_power_org);
	}

	//���� ����������� ������ ������
	if (bAddWound && GetHealth( ) > 0.0f)
	{
		return AddWound(hit_power * m_fWoundBoneScale, pHDS->hit_type, pHDS->boneID);
	}
	else
	{
		return NULL;
	}
}

f32 CEntityCondition::BleedingSpeed( )
{
	f32 bleeding_speed = 0.0f;

	for (WOUND_VECTOR_IT it = m_WoundVector.begin( ); m_WoundVector.end( ) != it; ++it)
	{
		bleeding_speed += (*it)->TotalSize( );
	}

	return (m_WoundVector.empty( ) ? 0.0f : bleeding_speed / m_WoundVector.size( ));
}

void CEntityCondition::UpdateHealth( )
{
	f32 bleeding_speed = BleedingSpeed( ) * m_fDeltaTime * m_change_v.m_fV_Bleeding;
	m_bIsBleeding = fis_zero(bleeding_speed) ? false : true;
	m_fDeltaHealth -= CanBeHarmed( ) ? bleeding_speed : 0.0f;
	m_fDeltaHealth += m_fDeltaTime * m_change_v.m_fV_HealthRestore;

	VERIFY(_valid(m_fDeltaHealth));
	ChangeBleeding(m_change_v.m_fV_WoundIncarnation * m_fDeltaTime);
}

void CEntityCondition::UpdatePower( )
{ }

void CEntityCondition::UpdatePsyHealth(f32 k)
{
	if (m_fPsyHealth > 0.0f)
	{
		m_fDeltaPsyHealth += m_change_v.m_fV_PsyHealth * k * m_fDeltaTime;
	}
}

void CEntityCondition::UpdateRadiation(f32 k)
{
	if (m_fRadiation > 0.0f)
	{
		m_fDeltaRadiation -= m_change_v.m_fV_Radiation * k * m_fDeltaTime;
		m_fDeltaHealth -= CanBeHarmed( ) ? m_change_v.m_fV_RadiationHealth * m_fRadiation * m_fDeltaTime : 0.0f;
	}
}

void CEntityCondition::UpdateEntityMorale( )
{
	if (m_fEntityMorale < m_fEntityMoraleMax)
	{
		m_fDeltaEntityMorale += m_change_v.m_fV_EntityMorale * m_fDeltaTime;
	}
}

bool CEntityCondition::IsLimping( ) const
{
	if (!m_use_limping_state)
	{
		return false;
	}

	return (m_fPower * GetHealth( ) <= m_limping_threshold);
}

void CEntityCondition::save(CNetPacket& output_packet)
{
	u8 is_alive = (GetHealth( ) > 0.0f) ? 1 : 0;

	output_packet.w_u8(is_alive);
	if (is_alive)
	{
		save_data(m_fPower, output_packet);
		save_data(m_fRadiation, output_packet);
		save_data(m_fEntityMorale, output_packet);
		save_data(m_fPsyHealth, output_packet);

		output_packet.w_u8((u8)m_WoundVector.size( ));
		for (WOUND_VECTOR_IT it = m_WoundVector.begin( ); m_WoundVector.end( ) != it; it++)
		{
			(*it)->save(output_packet);
		}
	}
}

void CEntityCondition::load(IReader& input_packet)
{
	m_bTimeValid = false;

	u8 is_alive = input_packet.r_u8( );
	if (is_alive)
	{
		load_data(m_fPower, input_packet);
		load_data(m_fRadiation, input_packet);
		load_data(m_fEntityMorale, input_packet);
		load_data(m_fPsyHealth, input_packet);

		ClearWounds( );
		m_WoundVector.resize(input_packet.r_u8( ));
		if (!m_WoundVector.empty( ))
		{
			for (u32 i = 0; i < m_WoundVector.size( ); i++)
			{
				CWound* pWound = xr_new<CWound>(BI_NONE);
				pWound->load(input_packet);
				m_WoundVector[i] = pWound;
			}
		}
	}
}

void CEntityCondition::SConditionChangeV::load(pcstr sect, pcstr prefix)
{
	string256				str;
	m_fV_Circumspection = 0.01f;

	strconcat(sizeof(str), str, "radiation_v", prefix);
	m_fV_Radiation = pSettings->r_float(sect, str);
	strconcat(sizeof(str), str, "radiation_health_v", prefix);
	m_fV_RadiationHealth = pSettings->r_float(sect, str);
	strconcat(sizeof(str), str, "morale_v", prefix);
	m_fV_EntityMorale = pSettings->r_float(sect, str);
	strconcat(sizeof(str), str, "psy_health_v", prefix);
	m_fV_PsyHealth = pSettings->r_float(sect, str);
	strconcat(sizeof(str), str, "bleeding_v", prefix);
	m_fV_Bleeding = pSettings->r_float(sect, str);
	strconcat(sizeof(str), str, "wound_incarnation_v", prefix);
	m_fV_WoundIncarnation = pSettings->r_float(sect, str);
	strconcat(sizeof(str), str, "health_restore_v", prefix);
	m_fV_HealthRestore = READ_IF_EXISTS(pSettings, r_float, sect, str, 0.0f);
}

void CEntityCondition::remove_links(const CObject* object)
{
	if (m_pWho != object)
	{
		return;
	}

	m_pWho = m_object;
	m_iWhoID = m_object->ID( );
}
