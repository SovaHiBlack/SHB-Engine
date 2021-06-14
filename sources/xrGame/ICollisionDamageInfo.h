#pragma once

struct SCollisionHitCallback;

class ICollisionDamageInfo
{
public:
	virtual float					ContactVelocity( ) const = 0;
	virtual void					HitDir(Fvector3& dir) const = 0;
	virtual const	Fvector3& HitPos( ) const = 0;
	virtual U16						DamageInitiatorID( ) const = 0;
	virtual CObject* DamageInitiator( ) const = 0;
	virtual ALife::EHitType			HitType( ) const = 0;
	virtual SCollisionHitCallback* HitCallback( ) const = 0;
	virtual void					Reinit( ) = 0;
};
