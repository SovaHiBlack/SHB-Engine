#pragma once

#include "ALife_space.h"

struct SHit
{
								SHit(float Power, Fvector3& dir, CObject* who, unsigned short element, Fvector3 p_in_object_space, float impulse, ALife::EHitType hit_type, float ap = 0.0f, bool AimBullet = false);
								SHit( );

	bool						is_valide				( ) const;
	void						invalidate				( );

	inline float				damage					( ) const
	{
		VERIFY(is_valide( ));
		return power;
	}
	inline const Fvector3&		direction				( ) const
	{
		VERIFY(is_valide( ));
		return dir;
	}
	inline const CObject*		initiator				( ) const
	{
		VERIFY(is_valide( ));
		return who;
	}
	inline unsigned short		bone					( ) const
	{
		VERIFY(is_valide( ));
		return boneID;
	}
	inline const Fvector3&		bone_space_position		( ) const
	{
		VERIFY(is_valide( ));
		return p_in_bone_space;
	}
	inline float				phys_impulse			( ) const
	{
		VERIFY(is_valide( ));
		return impulse;
	}
	inline ALife::EHitType		type					( ) const
	{
		VERIFY(is_valide( ));
		return hit_type;
	}

	void						Read_Packet				(CNetPacket P);
	void						Read_Packet_Cont		(CNetPacket P);
	void						Write_Packet			(CNetPacket& P);
	void						Write_Packet_Cont		(CNetPacket& P);

	void						GenHeader				(unsigned short PacketType, unsigned short ID);

	//GE_HIT
	unsigned int										Time;
	unsigned short										PACKET_TYPE;
	unsigned short										DestID;

	float												power;
	Fvector3											dir;
	CObject*											who;
	unsigned short										whoID;
	unsigned short										weaponID;
	unsigned short										boneID;
	Fvector3											p_in_bone_space;
	float												impulse;
	ALife::EHitType										hit_type;
	float												ap;
	bool												aim_bullet;

	//GE_HIT_STATISTIC
	unsigned int										BulletID;
	unsigned int										SenderID;

#ifdef DEBUG
	void						_dump					( );
#endif // def DEBUG

};
