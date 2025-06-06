#pragma once
#include "HudItemObject.h"
#include "HudSound.h"

#define MS_HIDDEN	0
#define MS_SHOWING	1
#define MS_IDLE		2
#define MS_THREATEN	3
#define MS_READY	4
#define MS_THROW	5
#define MS_END		6
#define MS_EMPTY	7
#define MS_HIDING	8
#define MS_PLAYING	9

struct dContact;
struct SGameMtl;

class CMissile : public CHudItemObject
{
	typedef CHudItemObject inherited;

public:
	CMissile( );
	virtual					~CMissile( );

	virtual BOOL			AlwaysTheCrow( )
	{
		return TRUE;
	}
	virtual void			OnDrawUI( );

	virtual void			reinit( );
	virtual CMissile* cast_missile( )
	{
		return this;
	}

	virtual void 			Load(pcstr section);
	virtual BOOL 			net_Spawn(CSE_Abstract* DC);
	virtual void 			net_Destroy( );

	virtual void 			UpdateCL( );
	virtual void 			shedule_Update(u32 dt);

	virtual void 			OnH_A_Chield( );
	virtual void 			OnH_B_Independent(bool just_before_destroy);

	virtual void 			OnEvent(CNetPacket& P, u16 type);

	virtual void 			OnAnimationEnd(u32 state);

	virtual void 			Show( );
	virtual void 			Hide( );
	virtual bool 			IsHidden( ) const
	{
		return (GetState( ) == MS_HIDDEN);
	}
	virtual bool 			IsHiding( ) const
	{
		return (GetState( ) == MS_HIDING);
	}
	virtual bool 			IsShowing( ) const
	{
		return (GetState( ) == MS_SHOWING);
	}

	virtual void 			Throw( );
	virtual void 			Destroy( );

	virtual bool 			Action(s32 cmd, u32 flags);

//.	IC u32		 			State						()				{return m_state;}
	virtual void 			State(u32 state);
	virtual void 			OnStateSwitch(u32 S);
	virtual void			GetBriefInfo(xr_string& str_name, xr_string& icon_sect_name, xr_string& str_count);

protected:
	virtual void			UpdateFireDependencies_internal( );
	virtual void			UpdateXForm( );
	void					UpdatePosition(const fMatrix4x4& trans);
	void					spawn_fake_missile( );

	//������������� ���� ���� � �������� ����� ��� �������� �� OnH_B_Chield
	virtual void			OnActiveItem( );
	virtual void			OnHiddenItem( );

	//��� ����
	virtual void			StartIdleAnim( );
	virtual void			net_Relcase(CObject* O);

protected:
	bool					m_throw;

	//����� �����������
	u32						m_dwDestroyTime;
	u32						m_dwDestroyTimeMax;

	fVector3					m_throw_direction;
	fMatrix4x4					m_throw_matrix;

	CMissile* m_fake_missile;

	//��������� ������

	f32 m_fMinForce;
	f32 m_fConstForce;
	f32 m_fMaxForce;
	f32 m_fForceGrowSpeed;

	bool					m_constpower;
	f32					m_fThrowForce;

protected:
	//������������� ����� � ����������� ������ �������
	fVector3					m_vThrowPoint;
	fVector3					m_vThrowDir;
	//��� HUD
	fVector3					m_vHudThrowPoint;
	fVector3					m_vHudThrowDir;

	//����� ��������
	shared_str				m_sAnimShow;
	shared_str				m_sAnimHide;
	shared_str				m_sAnimIdle;
	shared_str				m_sAnimPlaying;
	shared_str				m_sAnimThrowBegin;
	shared_str				m_sAnimThrowIdle;
	shared_str				m_sAnimThrowAct;
	shared_str				m_sAnimThrowEnd;

	//���� �������� "�������"
	SHudSound				sndPlaying;

protected:
	void			setup_throw_params( );

public:
	virtual void			activate_physic_shell( );
	virtual void			setup_physic_shell( );
	virtual void			create_physic_shell( );
	IC		void			set_destroy_time(u32 delta_destroy_time)
	{
		m_dwDestroyTime = delta_destroy_time + Device.dwTimeGlobal;
	}

protected:
	u32						m_ef_weapon_type;

public:
	virtual u32				ef_weapon_type( ) const;
	IC		u32				destroy_time( ) const
	{
		return m_dwDestroyTime;
	}
	static	void			ExitContactCallback(bool& do_colide, bool bo1, dContact& c, SGameMtl* /*material_1*/, SGameMtl* /*material_2*/);
	virtual u16				bone_count_to_synchronize( ) const;
};
