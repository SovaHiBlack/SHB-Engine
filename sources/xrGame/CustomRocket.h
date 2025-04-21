//////////////////////////////////////////////////////////////////////
// CustomRocket.h:	������, ������� �������� RocketLauncher 
//					(����� ������, ��������� � ���������� ��������)
//////////////////////////////////////////////////////////////////////

#pragma once

#include "physic_item.h"
#include "PHObject.h"

class CRocketLauncher;

struct SRoketContact
{
	bool contact;
	fVector3 pos;
	fVector3 up;
	SRoketContact()
	{
		contact = false;
	}
};

class CCustomRocket : public CPhysicItem,
					  public CPHUpdateObject
{
private:
	typedef CPhysicItem inherited;
	friend CRocketLauncher;

public:
	//////////////////////////////////////////////////////////////////////////
	//	Generic
	//////////////////////////////////////////////////////////////////////////

	CCustomRocket(void);
	virtual ~CCustomRocket(void);

	virtual void Load(pcstr section);
	virtual BOOL net_Spawn(CSE_Abstract* DC);
	virtual void net_Destroy();
	virtual BOOL						AlwaysTheCrow				()				{ return TRUE; }

	virtual void reinit		();
	virtual void reload		(pcstr section);

	virtual void OnH_A_Independent	();
	virtual void OnH_B_Independent	(bool just_before_destroy);
	virtual void OnH_B_Chield		();
	virtual void OnH_A_Chield		();
	virtual void UpdateCL();

	virtual BOOL UsedAI_Locations	()			{return	(FALSE);}
	virtual bool Useful				() const	{return (m_eState == eInactive);	}

	virtual void renderable_Render() {inherited::renderable_Render();}

	//�������� ���������� ��������
	virtual void			activate_physic_shell	();
	virtual void			create_physic_shell		();

	virtual void			PhDataUpdate			(f32 step);
	virtual void			PhTune					(f32 step);

	//////////////////////////////////////////////////////////////////////////
	//	Rocket Properties
	//////////////////////////////////////////////////////////////////////////
public:
#ifdef DEBUG
	CGameObject*			owner					(){return m_pOwner;}
#endif
	virtual	void			StartEngine				();
	virtual	void			StopEngine				();
	virtual	void			UpdateEngine			();
	virtual	void			UpdateEnginePh			();

	virtual	void			StartFlying				();
	virtual	void			StopFlying				();

	virtual	void			SetLaunchParams			(const fMatrix4x4& xform,
													 const fVector3& vel,
													 const fVector3& angular_vel);

	virtual void			OnEvent					(CNetPacket& P, u16 type);
	bool					m_bLaunched;
protected:
	//��������� �� ��������� RocketLauncher - ������� �������� ������
	CGameObject*			m_pOwner;

	SRoketContact			m_contact;
	//��������� ������� �������� RocketLauncher-�� ����� ������
	fMatrix4x4					m_LaunchXForm;
	fVector3					m_vLaunchVelocity;
	fVector3					m_vLaunchAngularVelocity;

	enum ERocketState{
		eInactive,		//������ ��������� � ���������� � ���������
		eEngine,		//������� ���������
		eFlying,		//������ �����
		eCollide		//��������� ������������ � ������������
	};

	//������� ��������� ������
	ERocketState m_eState;

	//��������� ������������
	bool		m_bEnginePresent;
	//����� ������ ��������� � ������� ������
	s32			m_dwEngineWorkTime;
	//���� ������ ��������� (������ �������� � �������)
	f32			m_fEngineImpulse;
	f32			m_fEngineImpulseUp;
	//������� ����� ������ ���������
	s32			m_dwEngineTime;

	//��������� ������������
	virtual void			Contact(const fVector3& pos, const fVector3& normal);
			void			PlayContact();
	static	void			ObjectContactCallback(bool& do_colide,bool bo1,dContact& c,SGameMtl * /*material_1*/,SGameMtl * /*material_2*/);


	//////////////////////////////////////////////////////////////////////////
	//	Lights
	//////////////////////////////////////////////////////////////////////////
protected:
	//����, ��� ��������� ����� ���� ��������
	bool			m_bLightsEnabled;
	//����, ��� ��������� ����� �����������
	//������ � ����������
	bool			m_bStopLightsWithEngine;
	//��������� �� ����� ������ � ������ ���������
	ref_light		m_pTrailLight;
	fColor			m_TrailLightColor;
	f32				m_fTrailLightRange;
	ref_sound		m_flyingSound;

protected:
	virtual void	StartLights();
	virtual void	StopLights();
	virtual void	UpdateLights();
    
	
	//////////////////////////////////////////////////////////////////////////
	//	Particles
	//////////////////////////////////////////////////////////////////////////
protected:	
	//��� ��������� ���������
	shared_str			m_sEngineParticles;
	CParticlesObject*	m_pEngineParticles;
	//��� ��������� ������
	shared_str			m_sFlyParticles;
	CParticlesObject*	m_pFlyParticles;

	fVector3				m_vPrevVel;
	f32					m_time_to_explode;

#ifdef	DEBUG
	f32					gbg_rocket_speed1;
	f32					gbg_rocket_speed2;
#endif

protected:
	virtual void		StartEngineParticles();
	virtual void		StopEngineParticles();
	virtual void		StartFlyParticles();
	virtual void		StopFlyParticles();

	virtual void		UpdateParticles();
};