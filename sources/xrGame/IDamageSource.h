#pragma once

class I_DamageSource
{
public:
	virtual void			SetInitiator(U16 id) = 0;
	virtual U16				Initiator( ) = 0;
	virtual I_DamageSource* cast_IDamageSource( )
	{
		return this;
	}
};
