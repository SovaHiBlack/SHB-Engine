// ========================================== SHB_Engine ==========================================
// Projekt		: Game
// Module		: DamagableItem.h
// Author		: Anahoret
// Description	: 
// ===================================== SovaHiBlack© - 2022 ======================================

#pragma once
#ifndef GAME_DAMAGABLEITEM_H_INCLUDED
#define GAME_DAMAGABLEITEM_H_INCLUDED

class CDamagableItem
{
protected:
			u16									m_levels_num;
			f32									m_max_health;
			u16									m_level_applied;

public:
						CDamagableItem			();
	virtual	void		Init					(f32 max_health, u16 level_num);
			void		HitEffect				();
			void		RestoreEffect			();
			f32			DamageLevelToHealth		(u16 dl);

protected:
			u16 		DamageLevel				();
	virtual	f32			Health					() = 0;
	virtual	void		ApplyDamage				(u16 level);
};

class CDamagableHealthItem : public CDamagableItem
{
	using inherited								= CDamagableItem;
			f32									m_health;

public:
	virtual	void		Init					(f32 max_health, u16 level_num);
			void		Hit						(f32 P);
			void		SetHealth				(f32 health)
	{
		m_health								= health;
	}

protected:
	virtual	f32			Health					()
	{
		return									m_health;
	}
};

#endif // ndef GAME_DAMAGABLEITEM_H_INCLUDED