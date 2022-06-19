// ========================================== SHB_Engine ==========================================
// Projekt		: Game
// Module		: DamagableItem.cpp
// Author		: Anahoret
// Description	: 
// ===================================== SovaHiBlack© - 2022 ======================================

#include "stdafx.h"
#include "DamagableItem.h"

CDamagableItem::CDamagableItem()
{
	m_max_health			= 0.0f;
	m_levels_num			= u16(-1);
	m_level_applied			= u16(-1);
}

u16 CDamagableItem::DamageLevel()
{
	f32 health				= Health();
	if (health < 0.0f)
	{
		health				= 0.0f;
	}

	u16 dl					= u16((1.0f - Health() / m_max_health) * m_levels_num);
	if (dl < m_levels_num)
	{
		return				dl;
	}
	else
	{
		return				m_levels_num;
	}
}

f32 CDamagableItem::DamageLevelToHealth(u16 dl)
{
	return					m_max_health * (f32(m_levels_num - dl) / m_levels_num);
}

void CDamagableItem::Init(f32 max_health, u16 level_num)
{
	m_max_health			= max_health;
	m_level_applied			= 0;
	m_levels_num			= level_num;
}

void CDamagableItem::HitEffect()
{
	u16 new_lewel			= DamageLevel();
	for (u16 i = m_level_applied + 1; i <= new_lewel; i++)
	{
		ApplyDamage			(i);
	}
}

void CDamagableItem::ApplyDamage(u16 level)
{
	m_level_applied			= level;
}

void CDamagableHealthItem::Init(f32 max_health, u16 level_num)
{
	inherited::Init			(max_health, level_num);
	m_health				= max_health;
}

void CDamagableHealthItem::Hit(f32 P)
{
	if (m_level_applied == m_levels_num)
	{
		return;
	}

	m_health				-= P;
	if (m_health < 0.0f)
	{
		m_health			= 0.0f;
	}

	HitEffect				();
}

void CDamagableItem::RestoreEffect()
{
	u16 dl					= DamageLevel();
	for (u16 i = 1; i <= dl; i++)
	{
		ApplyDamage			(i);
	}
}
