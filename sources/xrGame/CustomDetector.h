#pragma once

#include "InventoryItemObject.h"
#include "..\XR_3DA\feel_touch.h"
#include "hudsound.h"

class CCustomZone;
//�������� ���� ����
struct SZoneType
{
	//�������� ������ ����������� �����
	f32								min_freq;
	f32								max_freq;
	//���� ������� ��������� �� ���������� ����
	HUD_SOUND						detect_snds;

	shared_str						zone_map_location;
};

//�������� ����, ������������ ����������
struct SZoneInfo
{
	u32								snd_time;
	//������� ������� ������ �������
	f32								cur_freq;
	//particle for night-vision mode
	CParticlesObject*				pParticle;

	SZoneInfo( );
	~SZoneInfo( );
};

class CInventoryOwner;

class CCustomDetector : public CInventoryItemObject, public Feel::Touch
{
	typedef CInventoryItemObject	inherited;

public:
	CCustomDetector(void);
	virtual ~CCustomDetector(void);

	virtual BOOL net_Spawn(CSE_Abstract* DC);
	virtual void Load(pcstr section);

	virtual void OnH_A_Chield( );
	virtual void OnH_B_Independent(bool just_before_destroy);

	virtual void shedule_Update(u32 dt);
	virtual void UpdateCL( );

	virtual void feel_touch_new(CObject* O);
	virtual void feel_touch_delete(CObject* O);
	virtual BOOL feel_touch_contact(CObject* O);

	void TurnOn( );
	void TurnOff( );
	bool IsWorking( )
	{
		return m_bWorking;
	}

	virtual void OnMoveToSlot( );
	virtual void OnMoveToRuck( );
	virtual void OnMoveToBelt( );

protected:
	void StopAllSounds( );
	void UpdateMapLocations( );
	void AddRemoveMapSpot(CCustomZone* pZone, bool bAdd);
	void UpdateNightVisionMode( );

	bool m_bWorking;

	f32 m_fRadius;

	//���� ������ ������� �����
	CActor* m_pCurrentActor;
	CInventoryOwner* m_pCurrentInvOwner;

	//���������� �� ������������� �����
	DEFINE_MAP(CLASS_ID, SZoneType, ZONE_TYPE_MAP, ZONE_TYPE_MAP_IT);
	ZONE_TYPE_MAP m_ZoneTypeMap;

	//������ ������������ ��� � ���������� � ���
	DEFINE_MAP(CCustomZone*, SZoneInfo, ZONE_INFO_MAP, ZONE_INFO_MAP_IT);
	ZONE_INFO_MAP m_ZoneInfoMap;

	shared_str						m_nightvision_particle;

protected:
	u32					m_ef_detector_type;

public:
	virtual u32			ef_detector_type( ) const;
};
