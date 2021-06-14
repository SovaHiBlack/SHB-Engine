#pragma once

class CDamagableItem
{
protected:
	U16							m_levels_num;
	float						m_max_health;
	U16							m_level_applied;

public:
								CDamagableItem		( );
	virtual void				Init				(float max_health, U16 level_num);
	void						HitEffect			( );
	void						RestoreEffect		( );
	float						DamageLevelToHealth	(U16 dl);

protected:
	U16 						DamageLevel			( );
	virtual float				Health				( ) = 0;
	virtual void				ApplyDamage			(U16 level);
};

class CDamagableHealthItem : public CDamagableItem
{
	typedef CDamagableItem		inherited;
	float						m_health;

public:
	virtual void				Init				(float max_health, U16 level_num);
	void						Hit					(float P);
	void						SetHealth			(float health)
	{
		m_health = health;
	}

protected:
	virtual float				Health				( )
	{
		return m_health;
	}
};
