// ========================================== SHB_Engine ==========================================
// Projekt		: Game
// Module		: Hit.h
// Author		: Anahoret
// Description	: 
// ===================================== SovaHiBlack© - 2024 ======================================

#pragma once
#ifndef GAME_HIT_H_INCLUDED
#define GAME_HIT_H_INCLUDED

struct SHit
{
						SHit					(f32 Power, fVector3& Dir, CObject* Who, u16 Element, fVector3 PInBoneSpace, f32 Impulse, ALife::EHitType HitType, f32 aAP = 0.0f, bool AimBullet = false);
						SHit					( );

	bool				is_valide				( ) const;
	void				invalidate				( );

	IC f32				damage					( ) const
	{
		VERIFY									(is_valide( ));
		return power;
	}
	IC const fVector3&	direction				( ) const
	{
		VERIFY									(is_valide( ));
		return dir;
	}
	IC const CObject*	initiator				( ) const
	{
		VERIFY									(is_valide( ));
		return who;
	}
	IC u16				bone					( ) const
	{
		VERIFY									(is_valide( ));
		return boneID;
	}
	IC const fVector3&	bone_space_position		( ) const
	{
		VERIFY									(is_valide( ));
		return p_in_bone_space;
	}
	IC f32				phys_impulse			( ) const
	{
		VERIFY									(is_valide( ));
		return impulse;
	}
	IC ALife::EHitType	type					( ) const
	{
		VERIFY									(is_valide( ));
		return hit_type;
	}

	void				Read_Packet				(CNetPacket Packet);
	void				Read_Packet_Cont		(CNetPacket Packet);
	void				Write_Packet			(CNetPacket& Packet);
	void				Write_Packet_Cont		(CNetPacket& Packet);

	void				GenHeader				(u16 PacketType, u16 ID);
	//-----------------------------------------------------
	//GE_HIT
	u32											Time;
	u16											PACKET_TYPE;
	u16											DestID;

	f32											power;
	fVector3									dir;
	CObject*									who;
	u16											whoID;
	u16											weaponID;
	u16											boneID;
	fVector3									p_in_bone_space;
	f32											impulse;
	ALife::EHitType								hit_type;
	f32											ap;
	bool										aim_bullet;
	//-----------------------------------------------------
	//GE_HIT_STATISTIC
	u32											BulletID;
	u32											SenderID;

#ifdef DEBUG
	void				_dump					( );
#endif // def DEBUG

};

#endif // ndef GAME_HIT_H_INCLUDED