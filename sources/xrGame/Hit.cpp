// ========================================== SHB_Engine ==========================================
// Projekt		: Game
// Module		: Hit.cpp
// Author		: Anahoret
// Description	: 
// ===================================== SovaHiBlack© - 2024 ======================================

#include "stdafx.h"

#include "alife_space.h"
#include "Hit.h"
#include "ode_include.h"
#include "..\XR_3DA\Bone.h"
#include "../xrNetServer/net_utils.h"
#include "xrMessages.h"
#include "Level.h"

SHit::SHit(f32 Power, fVector3& Dir, CObject* Who, u16 Element, fVector3 PInBoneSpace, f32 Impulse, ALife::EHitType HitType, f32 AP, bool AimBullet)
{
	power					= Power;
	dir.set					(Dir);
	who						= Who;
	if (Who)
	{
		whoID				= Who->ID();
	}
	else
	{
		whoID				= 0;
	}

	boneID					= Element;
	p_in_bone_space.set		(PInBoneSpace);
	impulse					= Impulse;
	hit_type				= HitType;
	ap						= AP;
	PACKET_TYPE				= 0;
	BulletID				= 0;
	SenderID				= 0;
	aim_bullet				= AimBullet;
}
SHit::SHit()
{
	invalidate				();
}

bool SHit::is_valide() const
{
	return hit_type			!= ALife::eHitTypeMax;
}
void SHit::invalidate()
{
	Time					= 0;
	PACKET_TYPE				= 0;
	DestID					= 0;

	power					= -dInfinity;
	dir.set					(-dInfinity, -dInfinity, -dInfinity);
	who						= NULL;
	whoID					= 0;
	weaponID				= 0;

	boneID					= BI_NONE;
	p_in_bone_space.set		(-dInfinity, -dInfinity, -dInfinity);

	impulse					= -dInfinity;
	hit_type				= ALife::eHitTypeMax;

	ap						= 0.0f;
	BulletID				= 0;
	SenderID				= 0;
	aim_bullet				= false;
}

void SHit::Read_Packet(CNetPacket Packet)
{
	u16 type_dummy;
	Packet.r_begin			(type_dummy);
	Packet.r_u32			(Time);
	Packet.r_u16			(PACKET_TYPE);
	Packet.r_u16			(DestID);

	Read_Packet_Cont		(Packet);
}
void SHit::Read_Packet_Cont(CNetPacket Packet)
{
	Packet.r_u16			(whoID);
	Packet.r_u16			(weaponID);
	Packet.r_dir			(dir);
	Packet.r_float			(power);
	Packet.r_u16			(boneID);
	Packet.r_vec3			(p_in_bone_space);
	Packet.r_float			(impulse);
	aim_bullet				= Packet.r_u16() != 0;

	hit_type				= (ALife::EHitType)Packet.r_u16();			// hit type

	if (hit_type == ALife::eHitTypeFireWound)
	{
		Packet.r_float		(ap);
	}

	if (PACKET_TYPE == GE_HIT_STATISTIC)
	{
		Packet.r_u32		(BulletID);
		Packet.r_u32		(SenderID);
	}
}

void SHit::Write_Packet(CNetPacket& Packet)
{
	Packet.w_begin			(M_EVENT);
	Packet.w_u32			(Time);
	Packet.w_u16			(u16(PACKET_TYPE & 0xffff));
	Packet.w_u16			(u16(DestID & 0xffff));

	Write_Packet_Cont		(Packet);
}
void SHit::Write_Packet_Cont(CNetPacket& Packet)
{
	Packet.w_u16			(whoID);
	Packet.w_u16			(weaponID);
	Packet.w_dir			(dir);
	Packet.w_float			(power);
	Packet.w_u16			(boneID);
	Packet.w_vec3			(p_in_bone_space);
	Packet.w_float			(impulse);
	Packet.w_u16			(aim_bullet != 0);
	Packet.w_u16			(u16(hit_type & 0xffff));

	if (hit_type == ALife::eHitTypeFireWound)
	{
		Packet.w_float		(ap);
	}

	if (PACKET_TYPE == GE_HIT_STATISTIC)
	{
		Packet.w_u32		(BulletID);
		Packet.w_u32		(SenderID);
	}
}

void SHit::GenHeader(u16 PacketType, u16 ID)
{
	DestID					= ID;
	PACKET_TYPE				= PacketType;
	Time					= Level().timeServer();
}

#ifdef DEBUG
void SHit::_dump()
{
	Msg						("SHit::_dump()---begin");
	Log						("power=",				power);
	Log						("impulse=",			impulse);
	Log						("dir=",				dir);
	Log						("whoID=",				whoID);
	Log						("weaponID=",			weaponID);
	Log						("element=",			boneID);
	Log						("p_in_bone_space=",	p_in_bone_space);
	Log						("hit_type=",			(s32)hit_type);
	Log						("ap=",					ap);
	Msg						("SHit::_dump()---end");
}
#endif // def DEBUG
