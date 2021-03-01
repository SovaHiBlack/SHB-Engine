#pragma once

#include "MosquitoBald.h"

class CObjectAnimator;

class CTorridZone : public CMosquitoBald
{
private:
	using inherited							= CCustomZone;
	CObjectAnimator*						m_animator;

public:
					CTorridZone				( );
	virtual			~CTorridZone			( );
	virtual void	UpdateWorkload			(u32 dt);
	virtual void	shedule_Update			(u32 dt);
	BOOL			net_Spawn				(CSE_Abstract* DC);

	virtual bool	IsVisibleForZones		( )
	{
		return true;
	}
	virtual void	GoEnabledState			( );
	virtual void	GoDisabledState			( );
};
