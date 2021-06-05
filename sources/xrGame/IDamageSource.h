#pragma once

class I_DamageSource
{
public:
	virtual void			SetInitiator		(u16 id) = 0;
	virtual u16				Initiator			( ) = 0;
	virtual I_DamageSource*	cast_IDamageSource	( )
	{
		return this;
	}
};
