// Wound.h: класс описания раны

#pragma once

#include "ALife_space.h"
#include "HitImmunity_space.h"

class CNetPacket;

class CWound
{
public:
	CWound(unsigned short bone_num);
	virtual ~CWound( );

	//serialization
	virtual void save(CNetPacket& output_packet);
	virtual void load(IReader& input_packet);

	float	TotalSize( );
	float	TypeSize(ALife::EHitType hit_type);
	float	BloodSize( );

	void	AddHit(float hit_power, ALife::EHitType hit_type);

	//заживление раны
	void	Incarnation(float percent, float min_wound_size);
	unsigned short		GetBoneNum( )
	{
		return m_iBoneNum;
	}
	void 	SetBoneNum(unsigned short bone_num)
	{
		m_iBoneNum = bone_num;
	}

	unsigned short		GetParticleBoneNum( )
	{
		return m_iParticleBoneNum;
	}
	void	SetParticleBoneNum(unsigned short bone_num)
	{
		m_iParticleBoneNum = bone_num;
	}

	const CSharedString& GetParticleName( )
	{
		return m_sParticleName;
	}
	void	SetParticleName(CSharedString particle_name)
	{
		m_sParticleName = particle_name;
	}

	void  SetDestroy(bool destroy)
	{
		m_bToBeDestroy = destroy;
	}
	bool  GetDestroy( )
	{
		return m_bToBeDestroy;
	}

	//время обновления (для капель крови)
	float m_fDropTime;

protected:
	//косточка на которой появилась рана
	unsigned short m_iBoneNum;

	//косточка, если на ране отыгрывается партикл
	unsigned short m_iParticleBoneNum;
	//имя этого партикла
	CSharedString m_sParticleName;

	//список составляющих раны 
	HitImmunity::HitTypeSVec m_Wounds;

	bool		m_bToBeDestroy;
};
