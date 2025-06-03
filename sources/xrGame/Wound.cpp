// Wound.cpp: класс описания раны

#include "stdafx.h"
#include "Wound.h"
#include "../xrNetServer/net_utils.h"
#include "..\XR_3DA\Bone.h"

CWound::CWound(u16 bone_num)
{
	m_bToBeDestroy = false;

	m_Wounds.resize(ALife::eHitTypeMax);
	for (s32 i = 0; i < ALife::eHitTypeMax; i++)
	{
		m_Wounds[i] = 0.0f;
	}

	m_iBoneNum = bone_num;
	m_iParticleBoneNum = BI_NONE;

	m_fDropTime = 0.0f;
}

CWound::~CWound( )
{ }

#define WOUND_MAX 10.0f

//serialization
void CWound::save(CNetPacket& output_packet)
{
	output_packet.w_u8((u8) m_iBoneNum);
	for (s32 i = 0; i < ALife::eHitTypeMax; i++)
	{
		output_packet.w_float_q8(m_Wounds[i], 0.0f, WOUND_MAX);
	}
}

void  CWound::load(IReader& input_packet)
{
	m_iBoneNum = (u8) input_packet.r_u8( );
	for (s32 i = 0; i < ALife::eHitTypeMax; i++)
	{
		m_Wounds[i] = input_packet.r_float_q8(0.0f, WOUND_MAX);
		VERIFY(m_Wounds[i] >= 0.0f && m_Wounds[i] <= WOUND_MAX);
	}
}

f32 CWound::TotalSize( )
{
	f32 total_size = 0.f;
	for (s32 i = 0; i < ALife::eHitTypeMax; i++)
	{
		VERIFY(_valid(m_Wounds[i]));
		total_size += m_Wounds[i];
	}

	VERIFY(_valid(total_size));
	return total_size;
}

f32 CWound::TypeSize(ALife::EHitType hit_type)
{
	return m_Wounds[hit_type];
}

//кол-во кровавых ран
f32 CWound::BloodSize( )
{
	return (m_Wounds[ALife::eHitTypeWound] + m_Wounds[ALife::eHitTypeFireWound]);
}

void CWound::AddHit(f32 hit_power, ALife::EHitType hit_type)
{
	m_Wounds[hit_type] += hit_power;
	clamp(m_Wounds[hit_type], 0.0f, WOUND_MAX);
}

void CWound::Incarnation(f32 percent, f32 min_wound_size)
{
	f32 total_size = TotalSize( );

	if (fis_zero(total_size))
	{
		for (s32 i = 0; i < ALife::eHitTypeMax; i++)
		{
			m_Wounds[i] = 0.0f;
		}

		return;
	}

	//заживить все раны пропорционально их размеру
	for (s32 i = 0; i < ALife::eHitTypeMax; i++)
	{
		m_Wounds[i] -= percent/* *m_Wounds[i]*/;
		if (m_Wounds[i] < min_wound_size)
		{
			m_Wounds[i] = 0;
		}
	}
}
