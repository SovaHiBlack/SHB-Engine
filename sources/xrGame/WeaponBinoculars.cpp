#include "stdafx.h"
#include "WeaponBinoculars.h"

#include "xr_level_controller.h"

#include "Level.h"
#include "ui\UIFrameWindow.h"
#include "BinocularsVision.h"
#include "object_broker.h"
#include "hudmanager.h"

CWeaponBinoculars::CWeaponBinoculars( ) : CWeaponCustomPistol("BINOCULARS")
{
	m_binoc_vision = NULL;
	m_bVision = false;
}

CWeaponBinoculars::~CWeaponBinoculars( )
{
	SHudSound::DestroySound(sndZoomIn);
	SHudSound::DestroySound(sndZoomOut);
	xr_delete(m_binoc_vision);
}

void CWeaponBinoculars::Load(pcstr section)
{
	inherited::Load(section);

	// Sounds
	SHudSound::LoadSound(section, "snd_zoomin", sndZoomIn, eST_ITEM_USING);
	SHudSound::LoadSound(section, "snd_zoomout", sndZoomOut, eST_ITEM_USING);
	m_bVision = !!pSettings->r_bool(section, "vision_present");
}

bool CWeaponBinoculars::Action(s32 cmd, u32 flags)
{
	switch (cmd)
	{
		case kWPN_FIRE:
		{
			return inherited::Action(kWPN_ZOOM, flags);
		}
	}

	return inherited::Action(cmd, flags);
}

void CWeaponBinoculars::OnZoomIn( )
{
	if (H_Parent( ) && !IsZoomed( ))
	{
		SHudSound::StopSound(sndZoomOut);
		bool b_hud_mode = (Level( ).CurrentEntity( ) == H_Parent( ));
		SHudSound::PlaySound(sndZoomIn, H_Parent( )->Position( ), H_Parent( ), b_hud_mode);
		if (m_bVision && !m_binoc_vision)
		{
			//.VERIFY			(!m_binoc_vision);
			m_binoc_vision = xr_new<CBinocularsVision>(this);
		}
	}

	inherited::OnZoomIn( );
	m_fZoomFactor = m_fRTZoomFactor;
}

void CWeaponBinoculars::OnZoomOut( )
{
	if (H_Parent( ) && IsZoomed( ) && !IsRotatingToZoom( ))
	{
		SHudSound::StopSound(sndZoomIn);
		bool b_hud_mode = (Level( ).CurrentEntity( ) == H_Parent( ));
		SHudSound::PlaySound(sndZoomOut, H_Parent( )->Position( ), H_Parent( ), b_hud_mode);
		VERIFY(m_binoc_vision);
		xr_delete(m_binoc_vision);

		m_fRTZoomFactor = m_fZoomFactor;//store current
	}

	inherited::OnZoomOut( );
}

BOOL CWeaponBinoculars::net_Spawn(CSE_Abstract* DC)
{
	m_fRTZoomFactor = m_fScopeZoomFactor;
	inherited::net_Spawn(DC);
	return TRUE;
}

void CWeaponBinoculars::net_Destroy( )
{
	inherited::net_Destroy( );
	xr_delete(m_binoc_vision);
}

void CWeaponBinoculars::UpdateCL( )
{
	inherited::UpdateCL( );
	//manage visible entities here...
	if (H_Parent( ) && IsZoomed( ) && !IsRotatingToZoom( ) && m_binoc_vision)
	{
		m_binoc_vision->Update( );
	}
}

void CWeaponBinoculars::OnDrawUI( )
{
	if (H_Parent( ) && IsZoomed( ) && !IsRotatingToZoom( ) && m_binoc_vision)
	{
		m_binoc_vision->Draw( );
	}

	inherited::OnDrawUI( );
}

void GetZoomData(const f32 scope_factor, f32& delta, f32& min_zoom_factor)
{
	f32 def_fov = f32(g_fov);
	f32 min_zoom_k = 0.3f;
	f32 zoom_step_count = 3.0f;
	f32 delta_factor_total = def_fov - scope_factor;
	VERIFY(delta_factor_total > 0.0f);
	min_zoom_factor = def_fov - delta_factor_total * min_zoom_k;
	delta = (delta_factor_total * (1 - min_zoom_k)) / zoom_step_count;
}

void CWeaponBinoculars::ZoomInc( )
{
	f32 delta;
	f32 min_zoom_factor;
	GetZoomData(m_fScopeZoomFactor, delta, min_zoom_factor);

	m_fZoomFactor -= delta;
	clamp(m_fZoomFactor, m_fScopeZoomFactor, min_zoom_factor);
}

void CWeaponBinoculars::ZoomDec( )
{
	f32 delta;
	f32 min_zoom_factor;
	GetZoomData(m_fScopeZoomFactor, delta, min_zoom_factor);

	m_fZoomFactor += delta;
	clamp(m_fZoomFactor, m_fScopeZoomFactor, min_zoom_factor);
}

void CWeaponBinoculars::save(CNetPacket& output_packet)
{
	inherited::save(output_packet);
	save_data(m_fRTZoomFactor, output_packet);
}

void CWeaponBinoculars::load(IReader& input_packet)
{
	inherited::load(input_packet);
	load_data(m_fRTZoomFactor, input_packet);
}

void CWeaponBinoculars::GetBriefInfo(xr_string& str_name, xr_string& icon_sect_name, xr_string& str_count)
{
	str_name = NameShort( );
	str_count = "";
	icon_sect_name = *cNameSect( );
}

void CWeaponBinoculars::net_Relcase(CObject* object)
{
	if (!m_binoc_vision)
	{
		return;
	}

	m_binoc_vision->remove_links(object);
}

using namespace luabind;

void CWeaponBinoculars::script_register(lua_State* L)
{
	module(L)
		[
			class_<CWeaponBinoculars, CGameObject>("CWeaponBinoculars")
				.def(constructor<>( ))
		];
}
