//////////////////////////////////////////////////////////////////////////
// GraviZone.h:		гравитационна€ аномали€
//////////////////////////////////////////////////////////////////////////
//					состоит как бы из 2х зон
//					одна зат€гивает объект, друга€ взрывает и 
//					все неживые объекты (предметы и трупы)
//					поднимает в воздух и качает там какое-то
//					врем€
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

	//воздействие зоной на объект
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
	//сила импульса вт€гивани€ в зону (дл€ веса 100 кг)
	f32			m_fThrowInImpulse;
	//сила импульса вт€гивани€ в зону дл€ живых существ
	f32			m_fThrowInImpulseAlive;
	//коэфф. зат€гивани€ (чем меньше, тем плавнее зат€гивает)
	f32			m_fThrowInAtten;
	//радиус действи€ выброса (в процентах от всего)
	f32			m_fBlowoutRadiusPercent;

	//параметры телекинеза
	f32				m_fTeleHeight;
	u32				m_dwTimeToTele;
	u32				m_dwTelePause;
	u32				m_dwTeleTime;

	//им€ партиклов телекинеза
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