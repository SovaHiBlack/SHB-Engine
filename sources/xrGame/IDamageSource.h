#pragma once

class I_DamageSource
{
public:
	virtual void			SetInitiator(unsigned short id) = 0;
	virtual unsigned short				Initiator( ) = 0;
	virtual I_DamageSource* cast_IDamageSource( )
	{
		return this;
	}
};
