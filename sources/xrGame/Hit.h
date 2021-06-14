#pragma once

struct SHit
{
	SHit(float Power, Fvector3& dir, CObject* who, U16 element, Fvector3 p_in_object_space, float impulse, ALife::EHitType hit_type, float ap = 0.0f, bool AimBullet = false);
	SHit( );
	bool				is_valide( ) const;
	void				invalidate( );
	inline	float				damage( ) const
	{
		VERIFY(is_valide( ));
		return power;
	}
	inline const Fvector3& direction( ) const
	{
		VERIFY(is_valide( ));
		return dir;
	}
	inline const CObject* initiator( ) const
	{
		VERIFY(is_valide( ));
		return who;
	}
	inline			U16			bone( ) const
	{
		VERIFY(is_valide( ));
		return boneID;
	}
	inline	const Fvector3& bone_space_position( ) const
	{
		VERIFY(is_valide( ));
		return p_in_bone_space;
	}
	inline			float		phys_impulse( ) const
	{
		VERIFY(is_valide( ));
		return impulse;
	}
	inline	ALife::EHitType		type( )		const
	{
		VERIFY(is_valide( ));
		return hit_type;
	}
	void				Read_Packet(CNetPacket	P);
	void				Read_Packet_Cont(CNetPacket	P);
	void				Write_Packet(CNetPacket& P);
	void				Write_Packet_Cont(CNetPacket& P);

	void				GenHeader(U16 PacketType, U16 ID);

	//GE_HIT
	u32					Time;
	U16					PACKET_TYPE;
	U16					DestID;

	float				power;
	Fvector3				dir;
	CObject* who;
	U16					whoID;
	U16					weaponID;
	U16					boneID;
	Fvector3				p_in_bone_space;
	float				impulse;
	ALife::EHitType		hit_type;
	float				ap;
	bool				aim_bullet;
	//---------------------------------------------------
	//GE_HIT_STATISTIC
	u32					BulletID;
	u32					SenderID;

#ifdef DEBUG
	void				_dump( );
#endif

};
