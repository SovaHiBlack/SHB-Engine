#pragma once

class CVisualZone : public CCustomZone
{
	using inherited						= CCustomZone;

	MotionID							m_idle_animation;
	MotionID							m_attack_animation;
	u32									m_dwAttackAnimaionStart;
	u32									m_dwAttackAnimaionEnd;

public:
					CVisualZone			( );
	virtual			~CVisualZone		( );
	virtual BOOL	net_Spawn			(CSE_Abstract* DC);
	virtual void	net_Destroy			( );
	virtual void	AffectObjects		( );
	virtual void	SwitchZoneState		(EZoneState new_state);
	virtual void	Load				(const char* section);
	virtual void	UpdateBlowout		( );
};
