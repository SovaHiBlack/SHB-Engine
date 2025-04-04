#pragma once

struct SCollisionHitCallback;

class ICollisionDamageInfo
{
public:		
		virtual f32					ContactVelocity				()				const						=0;
		virtual void					HitDir(fVector3& dir)	const = 0;
		virtual const	fVector3&		HitPos						()				const						=0;
		virtual u16						DamageInitiatorID			()				const						=0;
		virtual CObject					*DamageInitiator			()				const						=0;
		virtual ALife::EHitType			HitType						()				const						=0;
		virtual SCollisionHitCallback	*HitCallback				()				const						=0;
		virtual void					Reinit						()											=0;
};
