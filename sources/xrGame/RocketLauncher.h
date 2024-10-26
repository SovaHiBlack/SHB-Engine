#pragma once

class CCustomRocket;
class CGameObject;

class CRocketLauncher
{
public:
	CRocketLauncher		();
	~CRocketLauncher	();

	virtual void Load	(pcstr section);

			void AttachRocket	(u16 rocket_id, CGameObject* parent_rocket_launcher);
			void DetachRocket	(u16 rocket_id, bool bLaunch);

			void SpawnRocket	(pcstr rocket_section, CGameObject* parent_rocket_launcher);
			void LaunchRocket	(const fMatrix4x4& xform,  const Fvector& vel, const Fvector& angular_vel);

protected:			   
	DEFINE_VECTOR(CCustomRocket*, ROCKET_VECTOR, ROCKETIT);
	ROCKET_VECTOR		m_rockets;
	ROCKET_VECTOR		m_launched_rockets;
	
	CCustomRocket*	getCurrentRocket();
	void			dropCurrentRocket();
	u32				getRocketCount();
	//��������� ��������, ����������� ������ �� �����
	//������
	f32			m_fLaunchSpeed;

};