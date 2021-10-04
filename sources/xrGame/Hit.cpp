#include "stdafx.h"

//#include "ALife_space.h"
#include "Hit.h"
#include "ode_include.h"
#include "..\ENGINE\bone.h"
#include "..\ENGINE\NetPacket.h"
#include "Messages.h"
#include "Level.h"

SHit::SHit(float aPower, Fvector3& adir, CObject* awho, unsigned short aelement, Fvector3 ap_in_bone_space, float aimpulse, ALife::EHitType ahit_type, float aAP, bool AimBullet)
{
	power					= aPower;
	dir.set					(adir);
	who						= awho;
	if (awho)
	{
		whoID				= awho->ID( );
	}
	else
	{
		whoID				= 0;
	}

	boneID					= aelement;
	p_in_bone_space.set		(ap_in_bone_space);
	impulse					= aimpulse;
	hit_type				= ahit_type;
	ap						= aAP;
	PACKET_TYPE				= 0;
	BulletID				= 0;
	SenderID				= 0;
	aim_bullet				= AimBullet;
}

SHit::SHit( )
{
	invalidate				( );
}

bool SHit::is_valide( ) const
{
	return (hit_type != ALife::eHitTypeMax);
}

void SHit::invalidate( )
{
	Time					= 0;
	PACKET_TYPE				= 0;
	DestID					= 0;

	power					= -dInfinity;
	dir.set					(-dInfinity, -dInfinity, -dInfinity);
	who						= nullptr;
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
	unsigned short type_dummy;
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
	aim_bullet				= Packet.r_u16( ) != 0;

	hit_type				= (ALife::EHitType)Packet.r_u16( );
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
	Packet.w_u16			(unsigned short(PACKET_TYPE & 0xffff));
	Packet.w_u16			(unsigned short(DestID & 0xffff));
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
	Packet.w_u16			(unsigned short(hit_type & 0xffff));

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

void SHit::GenHeader(unsigned short PacketType, unsigned short ID)
{
	DestID					= ID;
	PACKET_TYPE				= PacketType;
	Time					= Level( ).timeServer( );
}

#ifdef DEBUG
void SHit::_dump( )
{
	Msg("SHit::_dump()---begin");
	Log("power=",				power);
	Log("impulse=",				impulse);
	Log("dir=",					dir);
	Log("whoID=",				whoID);
	Log("weaponID=",			weaponID);
	Log("element=",				boneID);
	Log("p_in_bone_space=",		p_in_bone_space);
	Log("hit_type=",			(int) hit_type);
	Log("ap=",					ap);
	Msg("SHit::_dump()---end");
}
#endif // def DEBUG
