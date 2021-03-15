#pragma once

#include "Missile.h"
#include "IDamageSource.h"

class CBolt : public CMissile, public IDamageSource
{
	using inherited = CMissile;
	u16													m_thrower_id;

public:
								CBolt					( );
	virtual						~CBolt					( );

	virtual void				OnH_A_Chield			( );
	virtual void				OnEvent					(NET_Packet& P, u16 type);

	virtual bool				Activate				( );
	virtual void				Deactivate				( );

	virtual void				SetInitiator			(u16 id);
	virtual u16					Initiator				( );

	virtual void				Throw					( );
	virtual bool				Action					(int cmd, u32 flags);
	virtual bool				Useful					( ) const;
	virtual void				Destroy					( );
	virtual void				activate_physic_shell	( );

	virtual BOOL				UsedAI_Locations		( )
	{
		return FALSE;
	}
	virtual IDamageSource*		cast_IDamageSource		( )
	{
		return this;
	}
};
