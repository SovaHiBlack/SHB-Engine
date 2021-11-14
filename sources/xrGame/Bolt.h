#pragma once

#include "Missile.h"
#include "IDamageSource.h"

class CBolt : public CMissile, public I_DamageSource
{
	using inherited = CMissile;
	unsigned short													m_thrower_id;

public:
								CBolt					( );
	virtual						~CBolt					( );

	virtual void				OnH_A_Chield			( );
	virtual void				OnEvent					(CNetPacket& P, unsigned short type);

	virtual bool				Activate				( );
	virtual void				Deactivate				( );

	virtual void				SetInitiator			(unsigned short id);
	virtual unsigned short					Initiator				( );

	virtual void				Throw					( );
	virtual bool				Action					(int cmd, u32 flags);
	virtual bool				Useful					( ) const;
	virtual void				Destroy					( );
	virtual void				activate_physic_shell	( );

	virtual BOOL				UsedAI_Locations		( )
	{
		return FALSE;
	}
	virtual I_DamageSource*		cast_IDamageSource		( )
	{
		return this;
	}
};
