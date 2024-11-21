//////////////////////////////////////////////////////////////////////////
// GraviZone.h:		�������������� ��������
//////////////////////////////////////////////////////////////////////////
//					������� ��� �� �� 2� ���
//					���� ���������� ������, ������ �������� � 
//					��� ������� ������� (�������� � �����)
//					��������� � ������ � ������ ��� �����-��
//					�����
//////////////////////////////////////////////////////////////////////////
#pragma once

#include "customzone.h"
#include "ai/monsters/telekinesis.h"

class CBaseGraviZone : public CCustomZone
{
private:
	typedef		CCustomZone					inherited;

public:
	CBaseGraviZone( );
	virtual			~CBaseGraviZone( );

	virtual void	Load(pcstr section);

	virtual BOOL	net_Spawn(CSE_Abstract* DC);
	virtual void	net_Destroy( );
	virtual void	net_Relcase(CObject* O);

	//����������� ����� �� ������
	virtual void	Affect(SZoneObjectInfo* O);
	virtual void	AffectPull(CPhysicsShellHolder* GO, const fVector3& throw_in_dir, f32 dist);
	virtual void	AffectPullAlife(CEntityAlive* EA, const fVector3& throw_in_dir, f32 dist);
	virtual void	AffectPullDead(CPhysicsShellHolder* GO, const fVector3& throw_in_dir, f32 dist);
	virtual void	AffectThrow(SZoneObjectInfo* O, CPhysicsShellHolder* GO, const fVector3& throw_in_dir, f32 dist);
	virtual void	ThrowInCenter(fVector3& C);
	virtual bool	CheckAffectField(CPhysicsShellHolder* GO, f32 dist_to_radius);
	virtual void	shedule_Update(u32 dt);
	virtual bool	BlowoutState( );
	virtual bool	IdleState( );

	virtual f32	RelativePower(f32 dist);
	virtual f32	BlowoutRadiusPercent(CPhysicsShellHolder* /*GO*/)
	{
		return m_fBlowoutRadiusPercent;
	}

protected:
	virtual CTelekinesis& Telekinesis( ) = 0;

protected:
	//���� �������� ���������� � ���� (��� ���� 100 ��)
	f32			m_fThrowInImpulse;
	//���� �������� ���������� � ���� ��� ����� �������
	f32			m_fThrowInImpulseAlive;
	//�����. ����������� (��� ������, ��� ������� ����������)
	f32			m_fThrowInAtten;
	//������ �������� ������� (� ��������� �� �����)
	f32			m_fBlowoutRadiusPercent;

	//��������� ����������
	f32				m_fTeleHeight;
	u32				m_dwTimeToTele;
	u32				m_dwTelePause;
	u32				m_dwTeleTime;

	//��� ��������� ����������
	void			PlayTeleParticles(CGameObject* pObject);
	void			StopTeleParticles(CGameObject* pObject);

	shared_str		m_sTeleParticlesBig;
	shared_str		m_sTeleParticlesSmall;
};

class CGraviZone : public CBaseGraviZone
{
	typedef		CBaseGraviZone				inherited;
	CTelekinesis m_telekinesis;

protected:
	virtual CTelekinesis& Telekinesis( )
	{
		return m_telekinesis;
	}

public:
	CGraviZone( )
	{ }
	virtual				~CGraviZone( )
	{ }
};