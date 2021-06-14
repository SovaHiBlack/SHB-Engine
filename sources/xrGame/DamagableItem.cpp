#include "stdafx.h"

#include "DamagableItem.h"//ok

CDamagableItem::CDamagableItem( )
{
	m_max_health		= 0.0f;
	m_levels_num		= U16(-1);
	m_level_applied		= U16(-1);
}

void CDamagableItem::Init(float max_health, U16 level_num)
{
	m_max_health		= max_health;
	m_level_applied		= 0;
	m_levels_num		= level_num;
}

void CDamagableItem::HitEffect( )
{
	U16 new_lewel		= DamageLevel( );
	for (U16 i = m_level_applied + 1; i <= new_lewel; i++)
	{
		ApplyDamage		(i);
	}
}

void CDamagableItem::RestoreEffect( )
{
	U16 dl				= DamageLevel( );
	for (U16 i = 1; i <= dl; i++)
	{
		ApplyDamage		(i);
	}
}

float CDamagableItem::DamageLevelToHealth(U16 dl)
{
	return m_max_health * (float(m_levels_num - dl) / m_levels_num);
}

U16 CDamagableItem::DamageLevel( )
{
	float health		= Health( );
	if (health < 0.0f)
	{
		health			= 0.0f;
	}

	U16 dl				= U16((1.0f - Health( ) / m_max_health) * m_levels_num);

	if (dl < m_levels_num)
	{
		return			dl;
	}
	else
	{
		return			m_levels_num;
	}
}

void CDamagableItem::ApplyDamage(U16 level)
{
	m_level_applied		= level;
}

void CDamagableHealthItem::Init(float max_health, U16 level_num)
{
	inherited::Init		(max_health, level_num);
	m_health			= max_health;
}

void CDamagableHealthItem::Hit(float P)
{
	if (m_level_applied == m_levels_num)
	{
		return;
	}

	m_health			-= P;
	if (m_health < 0.0f)
	{
		m_health		= 0.0f;
	}

	HitEffect			( );
}
