#include "stdafx.h"
#include "CustomDetector.h"
#include "CustomZone.h"
#include "hudmanager.h"
#include "Artefact.h"
#include "Inventory.h"
#include "Level.h"
#include "MapManager.h"
#include "CameraEffector.h"
#include "Actor.h"
#include "ai_sounds.h"
#include "clsid_game.h"
#include "game_base_space.h"

SZoneInfo::SZoneInfo( )
{
	pParticle = NULL;
}

SZoneInfo::~SZoneInfo( )
{
	if (pParticle)
	{
		CParticlesObject::Destroy(pParticle);
	}
}

CCustomDetector::CCustomDetector( )
{
	m_bWorking = false;
}

CCustomDetector::~CCustomDetector( )
{
	ZONE_TYPE_MAP_IT it;
	for (it = m_ZoneTypeMap.begin( ); m_ZoneTypeMap.end( ) != it; ++it)
	{
		SHudSound::DestroySound(it->second.detect_snds);
	}

	m_ZoneInfoMap.clear( );
}

BOOL CCustomDetector::net_Spawn(CSE_Abstract* DC)
{
	m_pCurrentActor = NULL;
	m_pCurrentInvOwner = NULL;

	return inherited::net_Spawn(DC);
}

void CCustomDetector::Load(pcstr section)
{
	inherited::Load(section);

	m_fRadius = pSettings->r_float(section, "radius");

	if (pSettings->line_exist(section, "night_vision_particle"))
	{
		m_nightvision_particle = pSettings->r_string(section, "night_vision_particle");
	}

	u32 i = 1;
	string256 temp;

	//��������� ����� ��� ����������� ��������� ����� ���
	do
	{
		sprintf_s(temp, "zone_class_%d", i);
		if (pSettings->line_exist(section, temp))
		{
			pcstr z_Class = pSettings->r_string(section, temp);
			CLASS_ID zone_cls = TEXT2CLSID(pSettings->r_string(z_Class, "class"));

			m_ZoneTypeMap.insert(std::make_pair(zone_cls, SZoneType( )));
			SZoneType& zone_type = m_ZoneTypeMap[zone_cls];
			sprintf_s(temp, "zone_min_freq_%d", i);
			zone_type.min_freq = pSettings->r_float(section, temp);
			sprintf_s(temp, "zone_max_freq_%d", i);
			zone_type.max_freq = pSettings->r_float(section, temp);
			R_ASSERT(zone_type.min_freq < zone_type.max_freq);
			sprintf_s(temp, "zone_sound_%d_", i);

			SHudSound::LoadSound(section, temp, zone_type.detect_snds, eST_ITEM);

			sprintf_s(temp, "zone_map_location_%d", i);

			if (pSettings->line_exist(section, temp))
			{
				zone_type.zone_map_location = pSettings->r_string(section, temp);
			}

			++i;
		}
		else
		{
			break;
		}
	}
	while (true);

	m_ef_detector_type = pSettings->r_u32(section, "ef_detector_type");
}


void CCustomDetector::shedule_Update(u32 dt)
{
	inherited::shedule_Update(dt);

	if (!IsWorking( ))
	{
		return;
	}

	if (!H_Parent( ))
	{
		return;
	}

	Position( ).set(H_Parent( )->Position( ));

	if (H_Parent( ) && H_Parent( ) == Level( ).CurrentViewEntity( ))
	{
		fVector3 P;
		P.set(H_Parent( )->Position( ));
		feel_touch_update(P, m_fRadius);
		UpdateNightVisionMode( );
	}
}

void CCustomDetector::StopAllSounds( )
{
	ZONE_TYPE_MAP_IT it;
	for (it = m_ZoneTypeMap.begin( ); m_ZoneTypeMap.end( ) != it; ++it)
	{
		SZoneType& zone_type = (*it).second;
		SHudSound::StopSound(zone_type.detect_snds);
	}
}

void CCustomDetector::UpdateCL( )
{
	inherited::UpdateCL( );

	if (!IsWorking( ))
	{
		return;
	}

	if (!H_Parent( ))
	{
		return;
	}

	if (!m_pCurrentActor)
	{
		return;
	}

	ZONE_INFO_MAP_IT it;
	for (it = m_ZoneInfoMap.begin( ); m_ZoneInfoMap.end( ) != it; ++it)
	{
		CCustomZone* pZone = it->first;
		SZoneInfo& zone_info = it->second;

		//����� ��� ��� �� ��������������
		if (m_ZoneTypeMap.find(pZone->CLS_ID) == m_ZoneTypeMap.end( ) || !pZone->VisibleByDetector( ))
		{
			continue;
		}

		SZoneType& zone_type = m_ZoneTypeMap[pZone->CLS_ID];

		f32 dist_to_zone = H_Parent( )->Position( ).distance_to(pZone->Position( )) - 0.8f * pZone->Radius( );
		if (dist_to_zone < 0.0f)
		{
			dist_to_zone = 0.0f;
		}

		f32 fRelPow = 1.0f - dist_to_zone / m_fRadius;
		clamp(fRelPow, 0.0f, 1.0f);

		//���������� ������� ������� ������������ �������
		zone_info.cur_freq = zone_type.min_freq + (zone_type.max_freq - zone_type.min_freq) * fRelPow * fRelPow * fRelPow * fRelPow;

		f32 current_snd_time = 1000.0f * 1.0f / zone_info.cur_freq;

		if ((f32) zone_info.snd_time > current_snd_time)
		{
			zone_info.snd_time = 0;
			SHudSound::PlaySound(zone_type.detect_snds, fVector3( ).set(0.0f, 0.0f, 0.0f), this, true, false);
		}
		else
		{
			zone_info.snd_time += Device.dwTimeDelta;
		}
	}
}

void CCustomDetector::feel_touch_new(CObject* O)
{
	CCustomZone* pZone = smart_cast<CCustomZone*>(O);
	if (pZone && pZone->IsEnabled( ))
	{
		m_ZoneInfoMap[pZone].snd_time = 0;

		AddRemoveMapSpot(pZone, true);
	}
}

void CCustomDetector::feel_touch_delete(CObject* O)
{
	CCustomZone* pZone = smart_cast<CCustomZone*>(O);
	if (pZone)
	{
		m_ZoneInfoMap.erase(pZone);
		AddRemoveMapSpot(pZone, false);
	}
}

BOOL CCustomDetector::feel_touch_contact(CObject* O)
{
	return (NULL != smart_cast<CCustomZone*>(O));
}

void CCustomDetector::OnH_A_Chield( )
{
	m_pCurrentActor = smart_cast<CActor*>(H_Parent( ));
	m_pCurrentInvOwner = smart_cast<CInventoryOwner*>(H_Parent( ));
	inherited::OnH_A_Chield( );
}

void CCustomDetector::OnH_B_Independent(bool just_before_destroy)
{
	inherited::OnH_B_Independent(just_before_destroy);

	m_pCurrentActor = NULL;
	m_pCurrentInvOwner = NULL;

	StopAllSounds( );

	m_ZoneInfoMap.clear( );
	Feel::Touch::feel_touch.clear( );
}

u32 CCustomDetector::ef_detector_type( ) const
{
	return m_ef_detector_type;
}

void CCustomDetector::OnMoveToRuck( )
{
	inherited::OnMoveToRuck( );
	TurnOff( );
}

void CCustomDetector::OnMoveToSlot( )
{
	inherited::OnMoveToSlot( );
	TurnOn( );
}

void CCustomDetector::OnMoveToBelt( )
{
	inherited::OnMoveToBelt( );
	TurnOn( );
}

void CCustomDetector::TurnOn( )
{
	m_bWorking = true;
	UpdateMapLocations( );
	UpdateNightVisionMode( );
}

void CCustomDetector::TurnOff( )
{
	m_bWorking = false;
	UpdateMapLocations( );
	UpdateNightVisionMode( );
}

void CCustomDetector::AddRemoveMapSpot(CCustomZone* pZone, bool bAdd)
{
	if (m_ZoneTypeMap.find(pZone->CLS_ID) == m_ZoneTypeMap.end( ))
	{
		return;
	}

	if (bAdd && !pZone->VisibleByDetector( ))
	{
		return;
	}

	SZoneType& zone_type = m_ZoneTypeMap[pZone->CLS_ID];
	if (xr_strlen(zone_type.zone_map_location))
	{
		if (bAdd)
		{
			Level( ).MapManager( ).AddMapLocation(*zone_type.zone_map_location, pZone->ID( ));
		}
		else
		{
			Level( ).MapManager( ).RemoveMapLocation(*zone_type.zone_map_location, pZone->ID( ));
		}
	}
}

void CCustomDetector::UpdateMapLocations( ) // called on turn on/off only
{
	ZONE_INFO_MAP_IT it;
	for (it = m_ZoneInfoMap.begin( ); it != m_ZoneInfoMap.end( ); ++it)
	{
		AddRemoveMapSpot(it->first, IsWorking( ));
	}
}

void CCustomDetector::UpdateNightVisionMode( )
{
	bool bNightVision = Actor( )->Cameras( ).GetPPEffector(EEffectorPPType(effNightvision)) != NULL;

	bool bOn = bNightVision &&
		m_pCurrentActor &&
		m_pCurrentActor == Level( ).CurrentViewEntity( ) &&
		IsWorking( ) &&
		m_nightvision_particle.size( );

	ZONE_INFO_MAP_IT it;
	for (it = m_ZoneInfoMap.begin( ); m_ZoneInfoMap.end( ) != it; ++it)
	{
		CCustomZone* pZone = it->first;
		SZoneInfo& zone_info = it->second;

		if (bOn)
		{
			fVector3 zero_vector;
			zero_vector.set(0.0f, 0.0f, 0.0f);

			if (!zone_info.pParticle)
			{
				zone_info.pParticle = CParticlesObject::Create(*m_nightvision_particle, FALSE);
			}

			zone_info.pParticle->UpdateParent(pZone->XFORM( ), zero_vector);
			if (!zone_info.pParticle->IsPlaying( ))
			{
				zone_info.pParticle->Play( );
			}
		}
		else
		{
			if (zone_info.pParticle)
			{
				zone_info.pParticle->Stop( );
				CParticlesObject::Destroy(zone_info.pParticle);
			}
		}
	}
}
