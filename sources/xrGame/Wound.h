// Wound.h: ����� �������� ����

#pragma once

#include "alife_space.h"
#include "HitImmunity_space.h"

class CNetPacket;

class CWound
{
public:
	CWound				(u16 bone_num);
	virtual ~CWound		( );

	//serialization
	virtual void save	(CNetPacket& output_packet);
	virtual void load	(IReader &input_packet);

	f32	TotalSize	();
	f32	TypeSize	(ALife::EHitType hit_type);
	f32	BloodSize	();

	void	AddHit		(f32 hit_power, ALife::EHitType hit_type);
	
	//���������� ����
	void	Incarnation	(f32 percent, f32 min_wound_size);
	u16		GetBoneNum	() {return m_iBoneNum;}
	void 	SetBoneNum	(u16 bone_num) {m_iBoneNum = bone_num;}

	u16		GetParticleBoneNum	() {return m_iParticleBoneNum;}
	void	SetParticleBoneNum	(u16 bone_num) {m_iParticleBoneNum = bone_num;}

	const shared_str& GetParticleName	()						{return m_sParticleName;}
	void	SetParticleName	(shared_str particle_name) {m_sParticleName = particle_name;}

	void  SetDestroy(bool destroy) {m_bToBeDestroy = destroy;}
	bool  GetDestroy()			   {return m_bToBeDestroy;}

	//����� ���������� (��� ������ �����)
	f32 m_fDropTime;

protected:
	//�������� �� ������� ��������� ����
	u16 m_iBoneNum;

	//��������, ���� �� ���� ������������ �������
	u16 m_iParticleBoneNum;
	//��� ����� ��������
	shared_str m_sParticleName;

	//������ ������������ ���� 
	HitImmunity::HitTypeSVec m_Wounds;

	bool		m_bToBeDestroy;
};
