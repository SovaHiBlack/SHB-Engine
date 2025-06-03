#pragma once

class CDelayedActionFuse
{
	enum
	{
		flActive = 1 << 0,
		flInitialized = 1 << 1,
		flNoConditionChange = 1 << 2
	};
	flags8		m_dafflags;
	f32		m_fTime;
	f32		m_fSpeedChangeCondition;

public:
	void	SetTimer(f32 current_condition);
	void	Initialize(f32 time, f32 critical_condition);
	ICF bool	CheckCondition(f32 current_condition)
	{
		if (isInitialized( ) && !isActive( ) && m_fSpeedChangeCondition >= current_condition)
		{
			SetTimer(current_condition);
			return true;
		}
		else
		{
			return false;
		}
	}
	ICF bool	isActive( )
	{
		return !!m_dafflags.test(flActive);
	}
	ICF bool	isInitialized( )
	{
		return !!m_dafflags.test(flInitialized);
	}
	bool	Update(f32 current_condition);
	f32	Time( );

protected:
	CDelayedActionFuse( );
	virtual void	ChangeCondition(f32 fDeltaCondition) = 0;
	virtual void	StartTimerEffects( ) = 0;
};
