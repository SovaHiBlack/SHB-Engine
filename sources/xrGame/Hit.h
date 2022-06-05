// ========================================== SHB_Engine ==========================================
// Projekt		: Game
// Module		: Hit.h
// Author		: Anahoret
// Description	: 
// ===================================== SovaHiBlack© - 2022 ======================================

#pragma once
#ifndef GAME_HIT_H_INCLUDED
#define GAME_HIT_H_INCLUDED

struct SHit
{
						SHit					(F32 Power, Fvector& Dir, CObject* Who, u16 Element, Fvector PInBoneSpace, F32 Impulse, ALife::EHitType HitType, F32 aAP = 0.0f, bool AimBullet = false);
						SHit					();

	bool				is_valide				() const;
	void				invalidate				();

	IC F32				damage					() const
	{
		VERIFY			(is_valide());
		return			power;
	}
	IC const Fvector&	direction				() const
	{
		VERIFY			(is_valide());
		return			dir;
	}
	IC const CObject*	initiator				() const
	{
		VERIFY			(is_valide());
		return			who;
	}
	IC u16				bone					() const
	{
		VERIFY			(is_valide());
		return			boneID;
	}
	IC const Fvector&	bone_space_position		() const
	{
		VERIFY			(is_valide());
		return			p_in_bone_space;
	}
	IC F32				phys_impulse			() const
	{
		VERIFY			(is_valide());
		return			impulse;
	}
	IC ALife::EHitType	type					() const
	{
		VERIFY			(is_valide());
		return			hit_type;
	}

	void				Read_Packet				(NET_Packet Packet);
	void				Read_Packet_Cont		(NET_Packet Packet);
	void				Write_Packet			(NET_Packet& Packet);
	void				Write_Packet_Cont		(NET_Packet& Packet);

	void				GenHeader				(u16 PacketType, u16 ID);
	//---------------------------------------------------
	//GE_HIT
	u32											Time;
	u16											PACKET_TYPE;
	u16											DestID;

	F32											power;
	Fvector										dir;
	CObject*									who;
	u16											whoID;
	u16											weaponID;
	u16											boneID;
	Fvector										p_in_bone_space;
	F32											impulse;
	ALife::EHitType								hit_type;
	F32											ap;
	bool										aim_bullet;
	//---------------------------------------------------
	//GE_HIT_STATISTIC
	u32											BulletID;
	u32											SenderID;

#ifdef DEBUG
	void				_dump					();
#endif // def DEBUG

};

#endif // ndef GAME_HIT_H_INCLUDED