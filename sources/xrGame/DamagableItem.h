#pragma once

class CDamagableItem
{
protected:
	unsigned short							m_levels_num;
	float						m_max_health;
	unsigned short							m_level_applied;

public:
								CDamagableItem		( );
	virtual void				Init				(float max_health, unsigned short level_num);
	void						HitEffect			( );
	void						RestoreEffect		( );
	float						DamageLevelToHealth	(unsigned short dl);

protected:
	unsigned short 						DamageLevel			( );
	virtual float				Health				( ) = 0;
	virtual void				ApplyDamage			(unsigned short level);
};

class CDamagableHealthItem : public CDamagableItem
{
	typedef CDamagableItem		inherited;
	float						m_health;

public:
	virtual void				Init				(float max_health, unsigned short level_num);
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
