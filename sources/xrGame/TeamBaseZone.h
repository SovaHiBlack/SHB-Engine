//	Module 		: TeamBaseZone.h
//	Description : Team base zone object

#pragma once

#include "GameObject.h"
#include "..\ENGINE\feel_touch.h"

class CTeamBaseZone : public CGameObject, public Feel::Touch
{
protected:
	u8										m_Team;

public:
	using inherited							= CGameObject;

					CTeamBaseZone			( );
	virtual			~CTeamBaseZone			( );
	virtual void	reinit					( );
	virtual BOOL	net_Spawn				(CSE_Abstract* DC);
	virtual void	net_Destroy				( );

	virtual void	Center					(Fvector3& C) const;
	virtual float	Radius					( ) const;

	virtual void	shedule_Update			(u32 dt);
	virtual void	feel_touch_new			(CObject* O);
	virtual void	feel_touch_delete		(CObject* O);
	virtual BOOL	feel_touch_contact		(CObject* O);

	virtual u8		GetZoneTeam				( )
	{
		return m_Team;
	}

#ifdef DEBUG
	virtual void	OnRender				( );
#endif // def DEBUG

};
