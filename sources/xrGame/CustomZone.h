#pragma once

#include "SpaceRestrictor.h"
#include "..\XR_3DA\feel_touch.h"

class CActor;
class CLightAnimItem;
class CArtefact;
class CParticlesObject;
class CZoneEffector;

#define SMALL_OBJECT_RADIUS 0.6f

//���������� � �������, ����������� � ����
struct SZoneObjectInfo
{
	SZoneObjectInfo( ) :object(NULL), zone_ignore(false), time_in_zone(0), hit_num(0), total_damage(0), small_object(false), nonalive_object(false)
	{ }
	CGameObject* object;
	bool					small_object;
	bool					nonalive_object;
	//������������� ������� � ����
	bool					zone_ignore;
	//�������������� ��������
	xr_vector<CParticlesObject*>	particles_vector;
	//����� ���������� � ����
	u32						time_in_zone;
	//���������� ���, ������� ���� �������������� �� ������
	u32						hit_num;
	//���������� ����������� ���������� �����
	f32					total_damage;

	bool operator == (const CGameObject* O) const
	{
		return object == O;
	}
};

class CCustomZone : public CSpaceRestrictor, public Feel::Touch
{
private:
	typedef	CSpaceRestrictor inherited;

public:
	CZoneEffector* m_effector;

public:
	CCustomZone( );
	virtual				~CCustomZone( );

	virtual		BOOL	net_Spawn(CSE_Abstract* DC);
	virtual		void	net_Import(CNetPacket& P);
	virtual		void	net_Export(CNetPacket& P);
	virtual		void	Load(pcstr section);
	virtual		void	net_Destroy( );

	virtual		void	UpdateCL( );
	virtual		void	UpdateWorkload(u32	dt);				// related to fast-mode optimizations
	virtual		void	shedule_Update(u32	dt);
	virtual		void	enter_Zone(SZoneObjectInfo& io)
	{ }
	virtual		void	exit_Zone(SZoneObjectInfo& io);
	virtual		void	feel_touch_new(CObject* O);
	virtual		void	feel_touch_delete(CObject* O);
	virtual		BOOL	feel_touch_contact(CObject* O);
	virtual		BOOL	feel_touch_on_contact(CObject* O);
	virtual		f32		effective_radius( );
	virtual		f32		distance_to_center(CObject* O);
	virtual		void	Postprocess(f32 val)
	{ }
	virtual		void	net_Relcase(CObject* O);
	virtual		void	OnEvent(CNetPacket& P, u16 type);
	void	OnOwnershipTake(u16 id);

	f32		GetMaxPower( )
	{
		return m_fMaxPower;
	}
	void	SetMaxPower(f32 p)
	{
		m_fMaxPower = p;
	}

	//���������� ���� ���� � ����������� �� ���������� �� ������ ����
	//������������� ������ ���� (�� 0 �� 1)
	virtual		f32		RelativePower(f32 dist);
	//���������� ������
	f32		Power(f32 dist);

	virtual CCustomZone* cast_custom_zone( )
	{
		return this;
	}

	//��������� ��������� � ������� ����� ���������� ����
	typedef enum
	{
		eZoneStateIdle = 0,		//��������� ����, ����� ������ ��� ��� �������� ��������
		eZoneStateAwaking,		//����������� ���� (������ ����� � ����)
		eZoneStateBlowout,		//������
		eZoneStateAccumulate,	//������������ �������, ����� �������
		eZoneStateDisabled,
		eZoneStateMax
	} EZoneState;

protected:
	enum EZoneFlags
	{
		eIgnoreNonAlive = (1 << 0),
		eIgnoreSmall = (1 << 1),
		eIgnoreArtefact = (1 << 2),
		eVisibleByDetector = (1 << 3),
		eBlowoutWind = (1 << 4),
		eBlowoutLight = (1 << 5),
		eIdleLight = (1 << 6),
		eSpawnBlowoutArtefacts = (1 << 7),
		eUseOnOffTime = (1 << 8)
	};
	u32					m_owner_id;		//if created from artefact
	u32					m_ttl;
	flags32				m_zone_flags;
	//������ ��������, ����������� � ����
	CActor* m_pLocalActor;

	//������������ ���� ������ ����
	f32					m_fMaxPower;
	//�������� ����������� ��������� � ����������� �� ����������
	f32					m_fAttenuation;
	//������� ����� ����, ������� ������ �� ���������� �������
	f32					m_fHitImpulseScale;
	//������ ������� � ��������� �� �������������, ��� ��������� ����
	f32					m_fEffectiveRadius;

	//��� ���������� ����
	ALife::EHitType		m_eHitTypeBlowout;

	EZoneState			m_eZoneState;

	//������� ����� ���������� ���� � ������������ ��������� 
	s32					m_iStateTime;
	s32					m_iPreviousStateTime;

	u32					m_TimeToDisable;
	u32					m_TimeToEnable;
	u32					m_TimeShift;
	u32					m_StartTime;

	//������ � ���������, ������� ������ ��������� ������ 
	//������� (���� 0, �� ��������� -1 - �������������, 
	//-2 - ������ �� ������ ����������)
	typedef	svector<s32, eZoneStateMax>					StateTimeSVec;
	StateTimeSVec		m_StateTime;

	virtual		void		SwitchZoneState(EZoneState new_state);
	virtual		void		OnStateSwitch(EZoneState new_state);
	virtual		void		CheckForAwaking( );
	//��������� ���� � ��������� ����������
	virtual		bool		IdleState( );
	virtual		bool		AwakingState( );
	virtual		bool		BlowoutState( );
	virtual		bool		AccumulateState( );

	bool		Enable( );
	bool		Disable( );
	void		UpdateOnOffState( );
	virtual		void		GoEnabledState( );
	virtual		void		GoDisabledState( );

public:
	bool		IsEnabled( )
	{
		return (m_eZoneState != eZoneStateDisabled);
	}
	void		ZoneEnable( );
	void		ZoneDisable( );
	EZoneState				ZoneState( )
	{
		return m_eZoneState;
	}

protected:
	//����������� ����� �� ������
	virtual		void		Affect(SZoneObjectInfo* O)
	{ }

	//�������������� �� ��� ������� � ����
	virtual		void		AffectObjects( );

	u32						m_dwAffectFrameNum;

	u32						m_dwDeltaTime;
	u32						m_dwPeriod;

	//���� � ���� ���� �� disabled �������
	bool					m_bZoneActive;

	//��������� ��� �������, � ����� ��������� �������� ������� � ������
	u32						m_dwBlowoutParticlesTime;
	u32						m_dwBlowoutLightTime;
	u32						m_dwBlowoutSoundTime;
	u32						m_dwBlowoutExplosionTime;
	void					UpdateBlowout( );

	//�����
	bool					m_bBlowoutWindActive;
	u32						m_dwBlowoutWindTimeStart;
	u32						m_dwBlowoutWindTimePeak;
	u32						m_dwBlowoutWindTimeEnd;
	//���� ����� (���������� ��������) (0,1) ����� � �������� �������� �����
	f32						m_fBlowoutWindPowerMax;
	f32						m_fStoreWindPower;

	void					StartWind( );
	void					StopWind( );
	void					UpdateWind( );

	//�����, ����� �������, ���� �������� ����������� �� ������� ������ � ���� (-1 ���� �� �����)
	s32						m_iDisableHitTime;
	//�����, ����� �������, ���� �������� ����������� �� ��������� ������� ������ � ���� (-1 ���� �� �����)
	s32						m_iDisableHitTimeSmall;
	//����� ���������� idle ���������
	s32						m_iDisableIdleTime;

	////////////////////////////////
	// ����� ��������� ����

	//������� ��������� ����
	shared_str				m_sIdleParticles;
	//������ ����
	shared_str				m_sBlowoutParticles;
	shared_str				m_sAccumParticles;
	shared_str				m_sAwakingParticles;

	//��������� �������� � ���������� ������� � ����
	shared_str				m_sEntranceParticlesSmall;
	shared_str				m_sEntranceParticlesBig;
	//��������� �������� � ���������� ������� � ����
	shared_str				m_sHitParticlesSmall;
	shared_str				m_sHitParticlesBig;
	//���������� �������� � ���������� ������� � ����
	shared_str				m_sIdleObjectParticlesSmall;
	shared_str				m_sIdleObjectParticlesBig;
	BOOL					m_bIdleObjectParticlesDontStop;

	ref_sound				m_idle_sound;
	ref_sound				m_awaking_sound;
	ref_sound				m_accum_sound;
	ref_sound				m_blowout_sound;
	ref_sound				m_hit_sound;
	ref_sound				m_entrance_sound;

	//������ ��������� �������� ��������� ����
	CParticlesObject* m_pIdleParticles;

	//////////////////////////////
	//��������� ��������

	//��������� idle ���������
	ref_light				m_pIdleLight;
	fColor					m_IdleLightColor;
	f32						m_fIdleLightRange;
	f32						m_fIdleLightHeight;
	f32						m_fIdleLightRangeDelta;
	CLightAnimItem* m_pIdleLAnim;

	void					StartIdleLight( );
	void					StopIdleLight( );
	void					UpdateIdleLight( );

	//��������� �������
	ref_light				m_pLight;
	f32						m_fLightRange;
	fColor					m_LightColor;
	f32						m_fLightTime;
	f32						m_fLightTimeLeft;
	f32						m_fLightHeight;

	void					StartBlowoutLight( );
	void					StopBlowoutLight( );
	void					UpdateBlowoutLight( );

	//������ ��������� ��� ������� ������ ����
	DEFINE_VECTOR(SZoneObjectInfo, OBJECT_INFO_VEC, OBJECT_INFO_VEC_IT);
	OBJECT_INFO_VEC			m_ObjectInfoMap;

	void					CreateHit(u16 id_to,
									  u16 id_from,
									  const fVector3& hit_dir,
									  f32 hit_power,
									  s16 bone_id,
									  const fVector3& pos_in_bone,
									  f32 hit_impulse,
									  ALife::EHitType hit_type);


	virtual	void	Hit(SHit* pHDS);

	//��� ������������ ����
	void		PlayIdleParticles( );
	void		StopIdleParticles( );
	void		PlayAccumParticles( );
	void		PlayAwakingParticles( );
	void		PlayBlowoutParticles( );
	void		PlayEntranceParticles(CGameObject* pObject);
	void		PlayBulletParticles(fVector3& pos);

	void		PlayHitParticles(CGameObject* pObject);

	void		PlayObjectIdleParticles(CGameObject* pObject);
	void		StopObjectIdleParticles(CGameObject* pObject);

	virtual		bool		EnableEffector( )
	{
		return false;
	}

	virtual		bool		IsVisibleForZones( )
	{
		return false;
	}

	//����������, ���� ���� �������������
	virtual		void		OnMove( );
	fVector3					m_vPrevPos;
	u32						m_dwLastTimeMoved;

	//��������� ���� ����������
public:
	bool		VisibleByDetector( )
	{
		return !!m_zone_flags.test(eVisibleByDetector);
	}

//////////////////////////////////////////////////////////////////////////
// ������ ����������
protected:
	virtual			void	SpawnArtefact( );

	//�������� ��������� � ����, �� ����� �� ������������ � ������������� ��� � ����
	void	BornArtefact( );
	//������ ���������� �� ����
	void	ThrowOutArtefact(CArtefact* pArtefact);

	void	PrefetchArtefacts( );
	virtual BOOL		AlwaysTheCrow( );

protected:
	DEFINE_VECTOR(CArtefact*, ARTEFACT_VECTOR, ARTEFACT_VECTOR_IT);
	ARTEFACT_VECTOR			m_SpawnedArtefacts;

	//����������� ����, ��� �������� ������������ ��� ��������� ������������ ��������
	f32						m_fArtefactSpawnProbability;
	//�������� �������� ����������� ��������� �� ����
	f32						m_fThrowOutPower;
	//������ ��� ������� ����, ��� ����� ���������� ��������
	f32						m_fArtefactSpawnHeight;

	//��� ���������, ������� ������������� �� ����� � �� ����� �������� ���������
	shared_str				m_sArtefactSpawnParticles;
	//���� �������� ���������
	ref_sound				m_ArtefactBornSound;

	struct ARTEFACT_SPAWN
	{
		shared_str	section;
		f32		probability;
	};

	DEFINE_VECTOR(ARTEFACT_SPAWN, ARTEFACT_SPAWN_VECTOR, ARTEFACT_SPAWN_IT);
	ARTEFACT_SPAWN_VECTOR	m_ArtefactSpawn;

	//���������� �� ���� �� �������� ������
	f32					m_fDistanceToCurEntity;

protected:
	u32						m_ef_anomaly_type;
	u32						m_ef_weapon_type;
	BOOL					m_b_always_fastmode;

public:
	virtual u32				ef_anomaly_type( ) const;
	virtual u32				ef_weapon_type( ) const;
	virtual bool			register_schedule( ) const
	{
		return true;
	}

	// optimization FAST/SLOW mode
public:
	bool					o_fastmode;
	IC void					o_switch_2_fast( )
	{
		if (o_fastmode)
		{
			return;
		}

		o_fastmode = true;
		processing_activate( );
	}
	IC void					o_switch_2_slow( )
	{
		if (!o_fastmode)
		{
			return;
		}

		o_fastmode = false;
		processing_deactivate( );
	}
};
